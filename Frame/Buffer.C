#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "StringRoutines.h"
#include "Buffer.h"

/*****************************************************************************
	Constructors
*****************************************************************************/
Buffer::Buffer()
{
	fd = 0;
	file = 0;
	mark = 0;
	markIsSet = 0;
	bufferName = "No name buffer";
	size = 500;
	roomLeft = size;
	start = (char*)::calloc(size + 1,sizeof(char));
	end = start + size;
	current = start;
}

Buffer::Buffer(char *name, int buffersize)
{
	fd = 0;
	file = 0;
	mark = 0;
	markIsSet = 0;
	bufferName = name;
	size = buffersize;
	roomLeft = size;
	start = (char*)::calloc(size + 1,sizeof(char));
	end = start + size;
	current = start;
}

Buffer::Buffer(char *name)
{
	fd = 0;
	file = 0;
	mark = 0;
	markIsSet = 0;
	bufferName = name;
	size = 500;
	roomLeft = size;
	start = (char*)::calloc(size + 1,sizeof(char));
	end = start + size;
	current = start;
}

void Buffer::appendChar(char c, char *format, int width)
{
	if ( c )
		{
		if ( width > 10 )
			extend(width);
		else	extend(10);
		if ( !format )
			{
			if ( mark && mark < current )
				{
				shiftAtMark(1);
				*mark = c;
				}
			else {
				*current++ = c;
				*current = '\0';
				}
			}
		else {
			if ( mark && mark < current )
				{
				shiftAtMark(width);
				width = ::sprintf(mark,format,c);
				mark += width;
				}
			else {
				width = ::sprintf(current,format,c);
				current += width;
				}
			}
		roomLeft = (int)(end - current);
		}
	else	::fprintf(stderr,"Buffer: ERROR no character passed into appendChar\n");
}

void Buffer::appendDouble(double d, char *format, int width)
{
	if ( !format )
		format = "%f";
	if ( !width )
		width = 20;
	if ( mark )
		{
		char 	*formatted = (char*)::alloca(width + 1);
		int 	formattedLen = ::sprintf(formatted,format,d);
		insertAtMark(formatted,formattedLen);
		return;
		}
	extend(width);
	width = ::sprintf(current,format,d);
	current += width;
	roomLeft = (int)(end - current);
}

void Buffer::appendFloat(float f, char *format, int width)
{
	if ( !format )
		format = "%f";
	if ( !width )
		width = 20;
	if ( mark )
		{
		char 	*formatted = (char*)::alloca(width + 1);
		int 	formattedLen = ::sprintf(formatted,format,f);
		insertAtMark(formatted,formattedLen);
		return;
		}
	extend(width);
	width = ::sprintf(current,format,f);
	current += width;
	roomLeft = (int)(end - current);
}

void Buffer::appendInt(int i, char *format, int width)
{
	if ( !format )
		format = "%d";
	if ( !width )
		width = 10;
	if ( mark && mark < current )
		{
		char 	*formatted = (char*)::alloca(width + 1);
		int 	formattedLen = ::sprintf(formatted,format,i);
		insertAtMark(formatted,formattedLen);
		return;
		}
	extend(width);
	width = ::sprintf(current,format,i);
	current += width;
	roomLeft = (int)(end - current);
}

void Buffer::appendLong(long l, char *format, int width)
{
	if ( !format )
		format = "%lld";
	if ( !width )
		width = 20;
	if ( mark )
		{
		char 	*formatted = (char*)::alloca(width + 1);
		int 	formattedLen = ::sprintf(formatted,format,l);
		insertAtMark(formatted,formattedLen);
		return;
		}
	extend(width);
	width = ::sprintf(current,format,l);
	current += width;
	roomLeft = (int)(end - current);
}

/*****************************************************************************
	append methods. When mark is set, each appends as an insert-at-mark
    operation that also advances mark past the inserted bytes; otherwise the
    existing append-at-current behavior. The mark-aware path is what enables
    text-substrate incant directives (`buf += stringField` when buf has a mark).
*****************************************************************************/
void Buffer::appendString(char *text, char *format, int width)
{
	if ( text )
		{
		int 	length = width ? width : (int)::strlen(text);
		extend(length);
		if ( !format )
			{
			if ( mark && mark < current )
				insertAtMark(text,length);
			else
			while ( *text )
				{
				*current++ = *text++;
				}
			}
		else {
			if ( mark && mark < current )
				{
				shiftAtMark(length);
				length = ::sprintf(mark,format,text);
				mark += length;
				}
			else {
				length = ::sprintf(current,format,text);
				current += length;
				}
			}
		roomLeft = (int)(end - current);
		}
	else	::fprintf(stderr,"Buffer: ERROR no text passed into appendString\n");
}

/*****************************************************************************
        Back up the buffer to mark, if it is set.
*****************************************************************************/
void Buffer::backupToMark()
{
	if ( mark >= start && mark < current )
		{
		current = mark;
		*current = 0;
		roomLeft = (int)(end - current);
		}
}

/*****************************************************************************
    Flush and then close the file associated with the buffer then clear
    the fd and file fields.
*****************************************************************************/
int Buffer::closeFile()
{
int 	success = 0;
	flush();
	if ( fd )
		{
		success = ::close(fd);
		setFile((char*)0);
		fd = 0;
		if ( success < 0 )
			::printf("Could not close file\n");
		else	return 1;
		}
	else	::printf("closeFile: no file open\n");
	return 0;
}

/*****************************************************************************
    compress -- replace the buffer content with a self-describing, printable,
    NUL-free encoding of exactly the same bytes. decompress is its inverse.

    WHY PRINTABLE AND NOT RAW BINARY. Buffer is a string buffer: string(),
    toString(), tail() and appendString all assume the content carries no
    embedded zero byte, so a raw binary payload would leave the object in a
    state where its own accessors lie about it. The encoded form uses only
    A-Z a-z 0-9 - _ for its payload and digits and colons for its header, so
    it survives every Buffer accessor, every text envelope, and a URL query,
    with no escaping anywhere.

    WIRE FORM   BZ1:<mode>:<rawLen>:<payLen>:<payload>
        mode        z = LZSS packed, s = stored verbatim when packing lost
        rawLen      byte count of the ORIGINAL content
        payLen      byte count of the payload BEFORE armour
        payload     URL-safe base64, alphabet A-Z a-z 0-9 - _ , unpadded

    Reads start..current verbatim, so an embedded zero byte round trips.
    Clears any mark. Returns 1 on success, 0 with a message on failure.
*****************************************************************************/
int Buffer::compress()
{
char 	b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
char 	*raw = 0;
char 	*packed = 0;
char 	*armour = 0;
char 	*payload = 0;
char 	*header = 0;
int 	*head = 0;
int 	*prev = 0;
int 	rawLen = length();
int 	packedLen = 0;
int 	payLen = 0;
int 	armourLen = 0;
int 	at = 0;
int 	outAt = 0;
int 	flagAt = 0;
int 	flagBit = 0;
int 	winStart = 0;
int 	maxRun = 0;
int 	bestRun = 0;
int 	bestOff = 0;
int 	probe = 0;
int 	cand = 0;
int 	run = 0;
int 	code = 0;
int 	rest = 0;
int 	trio = 0;
int 	hash = 0;
int 	i = 0;
char 	mode = 0;
	if ( rawLen < 0 )
		rawLen = 0;
	raw = (char*)::malloc(rawLen + 2);
	if ( rawLen )
		::memcpy(raw,start,(size_t)rawLen);
	raw[rawLen] = 0;
	packed = (char*)::malloc(rawLen + rawLen / 8 + 32);
	head = (int*)::malloc(4096 * sizeof(int));
	prev = (int*)::malloc((rawLen + 2) * sizeof(int));
	for ( i = 0; i < 4096; i++ )
		head[i] = -1;
	at = 0;
	outAt = 0;
	flagAt = 0;
	flagBit = 8;
	while ( at < rawLen )
		{
		if ( flagBit == 8 )
			{
			flagAt = outAt;
			packed[outAt++] = 0;
			flagBit = 0;
			}
		bestRun = 0;
		bestOff = 0;
		maxRun = rawLen - at;
		if ( maxRun > 18 )
			maxRun = 18;
		winStart = at - 4096;
		if ( winStart < 0 )
			winStart = 0;
		if ( maxRun >= 3 )
			{
			hash = ((raw[at] & 0xFF) << 4) ^ ((raw[at + 1] & 0xFF) << 2) ^ (raw[at + 2] & 0xFF);
			hash &= 4095;
			cand = head[hash];
			probe = 0;
			while ( cand >= winStart && probe < 64 )
				{
				run = 0;
				while ( run < maxRun && raw[cand + run] == raw[at + run] )
					run++;
				if ( run > bestRun )
					{
					bestRun = run;
					bestOff = at - cand;
					}
				if ( bestRun >= maxRun )
					break;
				cand = prev[cand];
				probe++;
				}
			}
		if ( bestRun >= 3 )
			{
			code = ((bestOff - 1) << 4) | (bestRun - 3);
			packed[outAt++] = (char)((code >> 8) & 0xFF);
			packed[outAt++] = (char)(code & 0xFF);
			run = bestRun;
			}
		else {
			packed[flagAt] = (char)((packed[flagAt] & 0xFF) | (1 << flagBit));
			packed[outAt++] = raw[at];
			run = 1;
			}
		i = 0;
		while ( i < run )
			{
			if ( at + i + 2 < rawLen )
				{
				hash = ((raw[at + i] & 0xFF) << 4) ^ ((raw[at + i + 1] & 0xFF) << 2) ^ (raw[at + i + 2] & 0xFF);
				hash &= 4095;
				prev[at + i] = head[hash];
				head[hash] = at + i;
				}
			i++;
			}
		at += run;
		flagBit++;
		}
	packedLen = outAt;
	if ( packedLen < rawLen )
		{
		mode = 'z';
		payload = packed;
		payLen = packedLen;
		}
	else {
		mode = 's';
		payload = raw;
		payLen = rawLen;
		}
	rest = payLen % 3;
	armourLen = (payLen / 3) * 4;
	if ( rest )
		armourLen += rest + 1;
	armour = (char*)::malloc(armourLen + 8);
	i = 0;
	outAt = 0;
	while ( i + 2 < payLen )
		{
		trio = ((payload[i] & 0xFF) << 16) | ((payload[i + 1] & 0xFF) << 8) | (payload[i + 2] & 0xFF);
		armour[outAt++] = b64[(trio >> 18) & 63];
		armour[outAt++] = b64[(trio >> 12) & 63];
		armour[outAt++] = b64[(trio >> 6) & 63];
		armour[outAt++] = b64[trio & 63];
		i += 3;
		}
	if ( rest == 1 )
		{
		trio = (payload[i] & 0xFF) << 16;
		armour[outAt++] = b64[(trio >> 18) & 63];
		armour[outAt++] = b64[(trio >> 12) & 63];
		}
	else
	if ( rest == 2 )
		{
		trio = ((payload[i] & 0xFF) << 16) | ((payload[i + 1] & 0xFF) << 8);
		armour[outAt++] = b64[(trio >> 18) & 63];
		armour[outAt++] = b64[(trio >> 12) & 63];
		armour[outAt++] = b64[(trio >> 6) & 63];
		}
	armour[outAt] = 0;
	header = (char*)::malloc(64);
	::sprintf(header,"BZ1:%c:%d:%d:",mode,rawLen,payLen);
	reset();
	markIsSet = 0;
	appendString(header,0,0);
	if ( armourLen )
		appendString(armour,0,0);
	::free(raw);
	::free(packed);
	::free(armour);
	::free(header);
	::free(head);
	::free(prev);
	return 1;
}

/*****************************************************************************
    decompress -- exact inverse of compress. Parses the BZ1 header, un-armours
    the payload, unpacks it, and replaces the buffer content with the original
    bytes, verbatim, including any embedded zero byte.

    Refuses anything that is not a BZ1 stream rather than guessing, so calling
    it on ordinary content is a diagnosed no-op and not a corruption. Clears
    any mark. Returns 1 on success, 0 with a message on failure.
*****************************************************************************/
int Buffer::decompress()
{
char 	*scan = 0;
char 	*packed = 0;
char 	*raw = 0;
int 	rawLen = 0;
int 	payLen = 0;
int 	outAt = 0;
int 	bits = 0;
int 	nbits = 0;
int 	digit = 0;
int 	at = 0;
int 	from = 0;
int 	flags = 0;
int 	flagBit = 0;
int 	code = 0;
int 	off = 0;
int 	run = 0;
int 	ok = 0;
int 	i = 0;
char 	mode = 0;
char 	c = 0;
	if ( length() < 10 || ::strncmp(start,"BZ1:",(size_t)4) )
		{
		::fprintf(stderr,"Buffer decompress: content is not a BZ1 stream\n");
		return 0;
		}
	scan = start + 4;
	mode = *scan++;
	if ( mode != 'z' && mode != 's' )
		{
		::fprintf(stderr,"Buffer decompress: unknown mode\n");
		return 0;
		}
	if ( *scan++ != ':' )
		{
		::fprintf(stderr,"Buffer decompress: malformed header at mode\n");
		return 0;
		}
	rawLen = 0;
	while ( *scan >= '0' && *scan <= '9' )
		{
		rawLen = rawLen * 10 + (*scan - '0');
		scan++;
		}
	if ( *scan++ != ':' )
		{
		::fprintf(stderr,"Buffer decompress: malformed header at rawLen\n");
		return 0;
		}
	payLen = 0;
	while ( *scan >= '0' && *scan <= '9' )
		{
		payLen = payLen * 10 + (*scan - '0');
		scan++;
		}
	if ( *scan++ != ':' )
		{
		::fprintf(stderr,"Buffer decompress: malformed header at payLen\n");
		return 0;
		}
	if ( rawLen < 0 || payLen < 0 )
		{
		::fprintf(stderr,"Buffer decompress: header length out of range\n");
		return 0;
		}
	packed = (char*)::malloc(payLen + 8);
	outAt = 0;
	bits = 0;
	nbits = 0;
	ok = 1;
	while ( scan < current && ok )
		{
		c = *scan++;
		if ( c >= 'A' && c <= 'Z' )
			digit = c - 'A';
		else
		if ( c >= 'a' && c <= 'z' )
			digit = c - 'a' + 26;
		else
		if ( c >= '0' && c <= '9' )
			digit = c - '0' + 52;
		else
		if ( c == '-' )
			digit = 62;
		else
		if ( c == '_' )
			digit = 63;
		else {
			::fprintf(stderr,"Buffer decompress: bad payload character\n");
			ok = 0;
			}
		if ( ok )
			{
			bits = ((bits << 6) | digit) & 0xFFFFFF;
			nbits += 6;
			if ( nbits >= 8 )
				{
				nbits -= 8;
				if ( outAt < payLen )
					packed[outAt] = (char)((bits >> nbits) & 0xFF);
				outAt++;
				}
			}
		}
	if ( ok && outAt != payLen )
		{
		::fprintf(stderr,"Buffer decompress: payload length mismatch\n");
		ok = 0;
		}
	if ( !ok )
		{
		::free(packed);
		return 0;
		}
	raw = (char*)::malloc(rawLen + 2);
	if ( mode == 's' )
		{
		if ( payLen != rawLen )
			ok = 0;
		else
		if ( rawLen )
			::memcpy(raw,packed,(size_t)rawLen);
		}
	else {
		at = 0;
		i = 0;
		flags = 0;
		flagBit = 8;
		while ( at < rawLen && ok )
			{
			if ( flagBit == 8 )
				{
				if ( i >= payLen )
					ok = 0;
				else {
					flags = packed[i++] & 0xFF;
					flagBit = 0;
					}
				}
			if ( ok )
				{
				if (flags & (1 << flagBit))
					{
					if ( i >= payLen )
						ok = 0;
					else	raw[at++] = packed[i++];
					}
				else {
					if ( i + 1 >= payLen )
						ok = 0;
					else {
						code = ((packed[i] & 0xFF) << 8) | (packed[i + 1] & 0xFF);
						i += 2;
						off = (code >> 4) + 1;
						run = (code & 15) + 3;
						if ( off > at || at + run > rawLen )
							ok = 0;
						else {
							from = at - off;
							while ( run-- > 0 )
								raw[at++] = raw[from++];
							}
						}
					}
				flagBit++;
				}
			}
		}
	if ( !ok )
		{
		::fprintf(stderr,"Buffer decompress: corrupt payload\n");
		::free(packed);
		::free(raw);
		return 0;
		}
	reset();
	markIsSet = 0;
	extend(rawLen + 2);
	if ( rawLen )
		::memcpy(start,raw,(size_t)rawLen);
	current = start + rawLen;
	*current = 0;
	roomLeft = (int)(end - current);
	::free(packed);
	::free(raw);
	return 1;
}

/*****************************************************************************
    Delete number of characters from buffer. When markIsSet, deletes count
    chars starting at mark (left-shifts content after mark left by count);
    mark stays at the same absolute position so that what was at mark+count
    is now at mark. Otherwise the existing behavior: delete count chars
    walking back from current.
*****************************************************************************/
void Buffer::deleteFromBuffer(int count)
{
	if ( mark )
		{
		int 	tailLen = (int)(current - mark + count);
		if ( tailLen < 0 )
			tailLen = 0;
		memmove(mark,mark + count,(size_t)tailLen);
		current -= count;
		if ( current < mark )
			mark = 0;
		*current = 0;
		}
	else
	if ( count >= length() )
		reset();
	else
	if ( count <= 0 )
		::fprintf(stderr,"ERROR deleteFromBuffer: invalid number to delete %d\n",count);
	else {
		current -= count;
		*current = 0;
		if ( current < mark )
			mark = 0;
		}
	roomLeft = (int)(end - current);
}

/*****************************************************************************
        Extend the buffer to be able to accomodate additional input.
*****************************************************************************/
void Buffer::extend(int len)
{
	if ( len >= roomLeft )
		{
		int 	offset = 0;
		int 	upto = length();
		if ( mark )
			offset = (int)(mark - start);
		size = len + size * 2;
		roomLeft = size - upto;
		char 	*extent = (char*)::realloc(start,size);
		current = extent + upto;
		if ( offset )
			mark = extent + offset;
		start = extent;
		end = start + size;
		}
}

/*****************************************************************************
    Find a substring in the buffer. Scans from mark (if mark is set) or buffer
    start. On match: sets mark to start of match, sets markIsSet, returns 1.
    On no-match: leaves state unchanged, returns 0. This is the find-and-
    arm-mark primitive that opIN dispatches to for buffer targets.
*****************************************************************************/
int Buffer::findInBuffer(char *needle)
{
char 	*scanFrom = 0;
int 	needleLen = 0;
	if ( !needle || !*needle )
		return 0;
	scanFrom = mark ? mark : start;
	needleLen = (int)::strlen(needle);
	while ( *scanFrom )
		{
		if ( !::strncmp(scanFrom,needle,(size_t)needleLen) )
			{
			mark = scanFrom;
			markIsSet = 1;
			return 1;
			}
		scanFrom++;
		}
	return 0;
}

/*****************************************************************************
        Flush the file associated with the buffer and reset the buffer
*****************************************************************************/
void Buffer::flush()
{
	if ( !file )
		::fprintf(stderr,"Buffer flush: no file specified\n");
	else
	if ( !fd )
		fd = ::checkSys(::open(file,O_WRONLY | O_CREAT | O_TRUNC,0666),"Buffer flush: could not open file\n");
	if ( fd )
		{
		*current = 0;
		if ( current > start )
			{
			long 	success = ::write(fd,start,current - start);
			if (success < 0)
				{
				::printf("Problem writing buffer\n");
				::exit(1);
				}
			}
		reset();
		}
}

/*****************************************************************************
        Like toString but the returned string starts at the last mark.
*****************************************************************************/
char *Buffer::getMarkedString()
{
char 	*save = 0;
char 	*text = 0;
	if ( mark && mark < end )
		{
		save = start;
		start = mark;
		text = toString();
		start = save;
		}
	return text;
}

/*****************************************************************************
    Insert helper: insert length bytes from text at the mark, then advance
    mark past the inserted bytes. The chars previously at mark..current get
    shifted right. Used by mark-aware append-style methods when markIsSet.
    Callers must guarantee markIsSet is true (mark != 0 in well-formed Buffer).
*****************************************************************************/
void Buffer::insertAtMark(char *text, int length)
{
	if ( mark )
		{
		int 	tailLen = (int)(current - mark);
		extend(length);
		memmove(mark + length,mark,(size_t)tailLen);
		::memcpy(mark,text,(size_t)length);
		mark += length;
		current += length;
		*current = 0;
		roomLeft = (int)(end - current);
		}
	else	::fprintf(stderr,"Buffer: ERROR cannot insertAtMark because mark not set\n");
}

/*****************************************************************************
        Insert a string into buffer after the offset passed in. It is an error
        if the offset is greater than Buffer length and nothing happens.
*****************************************************************************/
void Buffer::insertIntoBuffer(char *text, int offset)
{
	if ( !text )
		::fprintf(stderr,"ERROR insertIntoBuffer: no text provided\n");
	else
	if ( offset >= 0 && offset < length() )
		{
		char 	*saveMark = mark;
		mark = start + offset;
		char 	*tail = getMarkedString();
		current = mark;
		mark = saveMark;
		this->appendString(text,0,0);
		this->appendString(tail,0,0);
		}
	else	::fprintf(stderr,"ERROR insertIntoBuffer: invalid offset %d\n",offset);
	roomLeft = (int)(end - current);
}

/*****************************************************************************
        Returns the number of characters in the buffer. Does not take into
		consideration any characters that have been flushed to a file.
*****************************************************************************/
int Buffer::length()
{
long 	length = current - start;
	if ( length > 0 )
		return (int)length;
	return 0;
}

/*****************************************************************************
        Frees the current buffer content and rebuilds it to have size = length
        Unlike extend, does not preserve current content.
*****************************************************************************/
void Buffer::reSize(int length)
{
	if ( size < length )
		{
		::free(start);
		size = length;
		roomLeft = size;
		start = (char*)::calloc(size + 100,sizeof(char));
		current = start;
		end = start + size;
		}
	reset();
}

/*****************************************************************************
        Resets the buffer (note, it does not flush the file if there is one)
*****************************************************************************/
void Buffer::reset()
{
	mark = 0;
	current = start;
	*current = 0;
	roomLeft = size;
}

/*****************************************************************************
        Associate a file with the buffer
*****************************************************************************/
void Buffer::setFile(char *name)
{
	file = name;
}

/*****************************************************************************
        Sets a mark in the buffer at the current position and arms the
        mark-aware machinery (markIsSet = true). Subsequent append and
        delete operations operate at/around mark; findInBuffer scans from
        mark forward. unMark disarms (clears markIsSet).
*****************************************************************************/
void Buffer::setMark()
{
	mark = current;
	markIsSet = 1;
}

/*****************************************************************************
    Shift text from mark right by length leaving mark as is. current is
    incremented by length. Text between mark and mark + length is left as is
    expected to be replaced.
*****************************************************************************/
void Buffer::shiftAtMark(int length)
{
	if ( mark )
		{
		int 	tailLen = (int)(current - mark);
		extend(length);
		memmove(mark + length,mark,(size_t)tailLen);
		current += length;
		*current = 0;
		}
	else	::fprintf(stderr,"Buffer: ERROR cannot shiftAtMark because mark not set\n");
}

/*****************************************************************************
    Drop the last count characters: back current up by count and re-terminate.
    Deliberately MARK-UNAWARE -- shorten always operates on the TAIL, where
    deleteFromBuffer diverts to a delete-at-mark when a mark is set. That
    divergence is why this is its own primitive rather than an alias: a caller
    that wants "give back what I just appended" must not have its meaning
    changed by whether some earlier code armed a mark.
    Underflow is a clamp, not an error: count at or past length() empties the
    buffer via reset(). A count of zero is a no-op so a back-off loop can call
    it without guarding. A negative count is a caller defect and says so.
*****************************************************************************/
void Buffer::shorten(int count)
{
	if ( count < 0 )
		::fprintf(stderr,"ERROR shorten: negative number to shorten %d\n",count);
	else
	if ( !count )
		return;
	else
	if ( count >= length() )
		reset();
	else {
		current -= count;
		*current = 0;
		if ( mark > current )
			{
			mark = 0;
			markIsSet = 0;
			}
		roomLeft = (int)(end - current);
		}
}

/*****************************************************************************
        Returns the start of the buffer after making sure it is 0 terminated
*****************************************************************************/
char *Buffer::string()
{
long 	length = current - start;
	*current = 0;
	if ( length > 0 )
		return start;
	return "";
}

/*****************************************************************************
        tabRight inserts count tabs into the buffer; used by incant print
        when indenting via ~ shortcut
*****************************************************************************/
void Buffer::tabRight(int count)
{
	if ( count )
		{
		extend(count);
		while ( count-- )
			{
			*current++ = '\t';
			}
		*current = '\0';
		roomLeft = (int)(end - current);
		}
}

/*****************************************************************************
        Returns a pointer to the string beginning at offset from current,
        or if buffer contains fewer characters than offset, then beginning
        at start. If buffer is empty, returns empty string.
*****************************************************************************/
char *Buffer::tail(int offset)
{
long 	length = current - start;
	if ( length > offset )
		return current - offset;
	else
	if ( length > 0 )
		return start;
	return "";
}

/*****************************************************************************
        Returns a string copy of the buffer
*****************************************************************************/
char *Buffer::toString()
{
long 	length = current - start;
char 	*text = 0;
	if ( length > 0 )
		{
		text = (char*)::malloc(length + 1);
		::strncpy(text,start,length);
		*(text + length) = 0;
		return text;
		}
	return "";
}

/*****************************************************************************
        Turn off the mark — disarms the mark-aware machinery so subsequent
        append/delete operations fall back to append-at-current behavior.
        Does NOT truncate the buffer (the prior behavior of truncating to
        mark was removed 2026-05-28; backupToMark is the explicit truncate
        operation). Mark pointer cleared so getMarkedString returns empty.
*****************************************************************************/
void Buffer::unMark()
{
	markIsSet = 0;
	mark = 0;
}
/*	Warning: the following methods were referenced but not declared
	memmove(char*,char*,size_t)
*/
