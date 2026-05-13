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
	bufferName = name;
	size = 500;
	roomLeft = size;
	start = (char*)::calloc(size + 1,sizeof(char));
	end = start + size;
	current = start;
}

void Buffer::appendChar(char c, char *format, int width)
{
	if ( !c )
		return;
	extend(10);
	if ( !format )
		{
		*current++ = c;
		*current = '\0';
		}
	else {
		if ( width > 10 )
			extend(width);
		width = ::sprintf(current,format,c);
		current += width;
		}
}

void Buffer::appendDouble(double d, char *format, int width)
{
	if ( !format )
		format = "%f";
	if ( !width )
		width = 20;
	extend(width);
	width = ::sprintf(current,format,d);
	current += width;
}

void Buffer::appendFloat(float f, char *format, int width)
{
	if ( !format )
		format = "%f";
	if ( !width )
		width = 20;
	extend(width);
	width = ::sprintf(current,format,f);
	current += width;
}

void Buffer::appendInt(int i, char *format, int width)
{
	if ( !format )
		format = "%d";
	if ( !width )
		width = 10;
	extend(width);
	width = ::sprintf(current,format,i);
	current += width;
}

void Buffer::appendLong(long l, char *format, int width)
{
	if ( !format )
		format = "%lld";
	if ( !width )
		width = 20;
	extend(width);
	width = ::sprintf(current,format,l);
	current += width;
}

/*****************************************************************************
	append methods
*****************************************************************************/
void Buffer::appendString(char *text, char *format, int width)
{
	if ( !text )
		return;
int length = (int)::strlen(text);
	if ( !format )
		{
		extend(length);
		while ( *text )
			{
			*current++ = *text++;
			}
		*current = '\0';
		}
	else {
		if ( width > length )
			extend(width);
		else	extend(length);
		length = ::sprintf(current,format,text);
		current += length;
		}
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
    Delete number of characters from buffer.
*****************************************************************************/
void Buffer::deleteFromBuffer(int count)
{
	if ( count >= length() )
		reset();
	else
	if ( count <= 0 )
		::fprintf(stderr,"ERROR deleteFromBuffer: invalid number to delete %d\n",count);
	else {
		current -= count;
		*current = 0;
		if ( mark < current )
			mark = 0;
		}
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
		start = (char*)::calloc(size + 100,sizeof(char));
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
}

/*****************************************************************************
        Associate a file with the buffer
*****************************************************************************/
void Buffer::setFile(char *name)
{
	file = name;
}

/*****************************************************************************
        Sets a mark in the buffer (marks are offsets from start)
*****************************************************************************/
void Buffer::setMark()
{
	mark = current;
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
			roomLeft--;
			}
		*current = '\0';
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
        Turn off the mark
*****************************************************************************/
void Buffer::unMark()
{
	if ( mark && mark != start )
		{
		current = mark;
		*current = 0;
		mark = 0;
		}
}
