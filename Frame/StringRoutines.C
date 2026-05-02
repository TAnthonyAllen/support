#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <unistd.h>
#include "PLGset.h"
#include "Buffer.h"
#include "StringRoutines.h"

/**************************************************************************
    Return a new string aligned left w/length passed in padded w/blanks
**************************************************************************/
extern "C" char *alignLeft(char *text, int length)
{
int 	textLength = (int)::strlen(text);
	if ( textLength >= length )
		return ::truncateString(text,length);
int 	padding = length - textLength;
char 	*aligned = (char*)::malloc(length + 1);
char 	*atText = aligned + textLength;
	::strcpy(aligned,text);
	while ( padding-- )
		{
		*atText++ = ' ';
		}
	*atText = 0;
	return aligned;
}

/**************************************************************************
    Return a new string aligned right w/length passed in padded w/blanks
**************************************************************************/
extern "C" char *alignRight(char *text, int length)
{
int 	textLength = (int)::strlen(text);
	if ( textLength >= length )
		return ::truncateString(text,length);
int 	padding = length - textLength;
char 	*aligned = (char*)::malloc(length + 1);
char 	*atText = aligned;
	while ( padding-- )
		{
		*atText++ = ' ';
		}
	::strcpy(atText,text);
	return aligned;
}

/*******************************************************************************
	This funky routine returns the number of bits set in the segment
*******************************************************************************/
extern "C" int bitCount(unsigned long bits)
{
int 	n = 0;
int 	result = 0;
	n = (int)bits;
	n = (n & 0x55555555) + ((n >> 1) & 0x55555555);
	n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
	n = (n + (n >> 4)) & 0x0f0f0f0f;
	n += n >> 8;
	n += n >> 16;
	result += n & 0xff;
	n = (int)(bits >> 32);
	n = (n & 0x55555555) + ((n >> 1) & 0x55555555);
	n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
	n = (n + (n >> 4)) & 0x0f0f0f0f;
	n += n >> 8;
	n += n >> 16;
	result += n & 0xff;
	return result;
}

/*****************************************************************************
	Is the boolean set? Used by the jit to access a bit with the parameters
    determined by the compiler at compile time.
*****************************************************************************/
extern "C" int booleanIsSet(unsigned char *item, int wordOffset, int bitOffset)
{
unsigned char 	*atItem = item + wordOffset;
unsigned long 	*atWord = (unsigned long*)atItem;
unsigned long 	mask = 1;
unsigned long 	result = 0;
	result = *atWord;
	result >>= bitOffset;
	return (int)(result &= mask);
}

/*****************************************************************************
	Is the button set? Used by the jit to access a button with the parameters
    determined by the compiler at compile time.
*****************************************************************************/
extern "C" int buttonIsSet(unsigned char *item, int wordOffset, int bitOffset, int length, int amount)
{
unsigned char 	*atItem = item + wordOffset;
unsigned long 	*atWord = (unsigned long*)atItem;
unsigned long 	mask = 0xFFFFFFFFFFFFFFFF;
unsigned long 	result = 0;
	result = *atWord;
	result >>= bitOffset;
	mask >>= 64 - length;
	result &= mask;
	if ( result == amount )
		return 1;
	return 0;
}

/**************************************************************************
	The checkSys routine is useful to test for errors when invoking
	a system call. Its first argument (an int) is the result of a
	system call and its second is the message to be displayed on 
	encountering an error, before closing up shop and going home (sort of
	gracefully). The routines that follow provide examples of its usage.
**************************************************************************/
extern "C" int checkSys(int returned, char *message)
{
	if ( returned == -1 )
		{
		::perror(message);
		::exit(2);
		}
	return (returned);
}

/**************************************************************************
    A wrapper for strcmp that handles null arguments. If left is less
    than right returns -1, equal returns 0, greater than 1
**************************************************************************/
extern "C" int compare(char *left, char *right)
{
int 	result = 0;
	if ( !left && !right )
		result = 0;
	else
	if ( !left )
		result = -1;
	else
	if ( !right )
		result = 1;
	else	result = ::strcmp(left,right);
	return result;
}

/**************************************************************************
    A wrapper for strncmp that handles null arguments and works on a stream
    argument. If left less than right returns -1, equal returns 0, greater than 1
**************************************************************************/
extern "C" int compareToStream(char *left, char *right)
{
int 	result = 0;
	if ( !left && !right )
		result = 0;
	else
	if ( !left )
		result = -1;
	else
	if ( !right )
		result = 1;
	else	result = ::strncmp(left,right,::strlen(left));
	return result;
}

/**************************************************************************
    Concatenate a list of strings
**************************************************************************/
extern "C" char *concat(int count, ...)
{
va_list 	list;
int 		i = 0;
int 		length = 0;
char 		*result = 0;
char 		*atResult = 0;
char 		**text = 0;
char 		**atText = 0;
	atText = text = (char**)::alloca((count + 1) * sizeof(char*));
	*(text + count) = 0;
	::va_start(list,count);
	for ( i = 0; i < count; i++ )
		{
		*atText = va_arg(list,char*);
		if (*atText)
			{
			length += (int)::strlen(*atText);
			atText++;
			}
		}
	::va_end(list);
	atResult = result = (char*)::calloc(length + 1,sizeof(char*));
	for ( atText = text; atText && *atText; atText++ )
		{
		::strcpy(atResult,*atText);
		atResult += ::strlen(*atText);
		}
	return result;
}

/*******************************************************************************
	Returns true if text passed in contains the character passed in.
*******************************************************************************/
extern "C" int containsCharacter(char *text, char c)
{
char 	*atText = text;
	while ( *atText )
		if ( *atText == c )
			return 1;
		else	atText++;
	return 0;
}

/*******************************************************************************
	Returns true if text passed in contains the substring passed in.
*******************************************************************************/
extern "C" int containsString(char *text, char *substring)
{
char 	*atText = text;
int 	length = (int)::strlen(substring);
	while ( *atText )
		if ( !::strncmp(atText,substring,length) )
			break;
		else	atText++;
	if ( *atText )
		return 1;
	return 0;
}

/**************************************************************************
    Replace escaped characters in the string passed in
**************************************************************************/
extern "C" void convertNewLine(char *text)
{
char 	*at = 0;
char 	*target = 0;
char 	*ff = "\f";
char 	*nl = "\n";
char 	*rt = "\r";
char 	*tb = "\t";
	for ( at = target = text; target && *target; at++, target++ )
		{
		if ( *target == '\\' )
			{
			target++;
			switch (*target)
				{
				case 'f':
					*at = *ff;
					break;
				case 'n':
					*at = *nl;
					break;
				case 'r':
					*at = *rt;
					break;
				case 't':
					*at = *tb;
					break;
				}
			}
		else	*at = *target;
		}
	if ( at < target )
		{
		*at = 0;
		}
}

/**************************************************************************
    Returns a copy of the string passed in.
**************************************************************************/
extern "C" char *copyString(char *input)
{
char 	*result = 0;
	if ( input && *input )
		{
		int 	length = (int)::strlen(input);
		result = (char*)::calloc(length + 1,sizeof(char));
		::strncpy(result,input,(size_t)length);
		}
	return result;
}

/******************************************************************************
    Returns text contents of a mapped  file
******************************************************************************/
extern "C" char *getStringFromFile(char *inputFile)
{
long 	length = 0;
long 	increment = 0;
int 	file = 0;
char 	*input = 0;
	file = ::open(inputFile,O_RDWR);
	if ( file > 0 )
		{
		length = ::lseek(file,0,SEEK_END);
		increment = length + 4096;
		// The following kludginess appears because mmap craps out when
		// file ends on page size boundary
		if ( length % 4096 == 0 )
			{
			input = (char*)::malloc(increment);
			::lseek(file,0,SEEK_SET);
			increment = read(file,input,length);
			if ( increment != length )
				::fprintf(stderr,"getStringFromFile: Problem reading in %s\n",inputFile);
			}
		else	input = (char*)::mmap(0,increment,PROT_READ | PROT_WRITE,MAP_PRIVATE,file,0);
		if ( (long)input == -1 )
			::fprintf(stderr,"getStringFromFile: could not mmap file: %s\n",inputFile);
		::close(file);
		*(input + length) = '\0';
		}
	else {
		char 	*errorMessage = ::concat(2,"getStringFromFile: could not open file: ",inputFile);
		::checkSys(file,errorMessage);
		::fprintf(stderr,"\tcurrent directory: ");
		::system("pwd");
		}
	return input;
}

/**************************************************************************
    Returns the first part of input up to the first character that matches
    the match character
**************************************************************************/
extern "C" char *headToChar(char *input, char match)
{
char 	*atInput = 0;
int 	length = 0;
	if ( input && *input )
		{
		atInput = input;
		while ( *atInput )
			if ( *atInput++ == match )
				break;
			else	length++;
		if ( length )
			{
			atInput = (char*)::calloc(length + 1,sizeof(char));
			::strncpy(atInput,input,(size_t)length);
			}
		else	atInput = 0;
		}
	return atInput;
}

/*****************************************************************************
	Return the first count characters of text as a new string.
*****************************************************************************/
extern "C" char *headToCount(char *text, int count)
{
int 	length = (int)::strlen(text);
	if ( length > count && count > 0 )
		{
		char 	*buffer = (char*)::calloc(count + 1,sizeof(char));
		if ( text && *text )
			::strncpy(buffer,text,(size_t)count);
		return buffer;
		}
	return text;
}

/**************************************************************************
    Returns the first part of input up to the first character that matches
    the the character set passed in
**************************************************************************/
extern "C" char *headToSet(char *input, PLGset *matchSet)
{
char 	*atInput = 0;
int 	length = 0;
	if ( input && *input )
		{
		atInput = input;
		while ( *atInput )
			if ( matchSet->contains(*atInput++) )
				break;
			else	length++;
		if ( length )
			{
			atInput = (char*)::calloc(length + 1,sizeof(char));
			::strncpy(atInput,input,(size_t)length);
			}
		else	atInput = 0;
		}
	return atInput;
}

/**************************************************************************
    Returns the first part of input up to the part that matches match
**************************************************************************/
extern "C" char *headToString(char *input, char *match)
{
char 	*atInput = 0;
char 	*result = 0;
	if ( input && *input )
		{
		int 	length = (int)::strlen(input) - (int)::strlen(match);
		atInput = input + length;
		if ( ::compare(atInput,match) != 0 )
			return 0;
		result = (char*)::calloc(length + 1,sizeof(char));
		::strncpy(result,input,(size_t)length);
		return result;
		}
	return 0;
}

/*****************************************************************************
    Writes out indent (often used in debugging directives). Note: this appears
    at the end of this code file so that the print(beffer) command does not
    pooch subsequent print code.
*****************************************************************************/
extern "C" void indent(int indentify, char *indenter, Buffer *indentBuffer)
{
int 	i = indentify > 0 ? indentify : 0;
int 	indentLength = 0;
	if ( i == 0 )
		return;
	if ( !indenter )
		indenter = "\t";
	indentLength = (int)::strlen(indenter);
char 	*indenting = (char*)::calloc(1,indentify * indentLength + 1);
char 	*atText = indenting;
	while ( i-- > 0 )
		{
		::strcpy(atText,indenter);
		atText += indentLength;
		}
	if ( !indentBuffer )
		::printf("%s",indenting);
	else	indentBuffer->appendString(indenting);
}

/*****************************************************************************
	Set boolean value. Used by the jit to set a boolean with the parameters
    determined by the compiler at compile time.
*****************************************************************************/
extern "C" void setBoolean(unsigned char *item, int wordOffset, int bitOffset, int value)
{
unsigned char 	*atItem = item + wordOffset;
unsigned long 	*atWord = (unsigned long*)atItem;
unsigned long 	mask = 1 << bitOffset;
unsigned long 	result = 0;
	mask &= *atWord;
	if ( value )
		{
		result = value << bitOffset;
		if ( mask )
			return;
		else	*atWord ^= result;
		}
	else
	if ( !mask )
		return;
	else	*atWord ^= mask;
}

/*****************************************************************************
	Set button value. Used by the jit to set a button with the parameters
    determined by the compiler at compile time.
*****************************************************************************/
extern "C" void setButton(unsigned char *item, int wordOffset, int bitOffset, int length, int amount)
{
unsigned char 	*atItem = item + wordOffset;
unsigned long 	*atWord = (unsigned long*)atItem;
unsigned long 	mask = 0xFFFFFFFFFFFFFFFF;
unsigned long 	result = 0;
	result = amount;
	mask >>= 64 - length;
	mask <<= bitOffset;
	result <<= bitOffset;
	// The following two lines clear the button
	mask &= *atWord;
	*atWord ^= mask;
	// The following sets the button
	*atWord |= result;
}

/*******************************************************************************
	Returns true if text passed in is numeric.
*******************************************************************************/
extern "C" int stringIsNumeric(char *text)
{
char 	*atText = text;
int 	hasDecimalPoint = 0;
	while ( *atText )
		if ( *atText >= '0' && *atText <= '9' )
			atText++;
		else
		if ( *atText == '.' )
			if ( hasDecimalPoint )
				return 0;
			else {
				hasDecimalPoint = 1;
				atText++;
				}
		else	return 0;
	return 1;
}

/**************************************************************************
    Returns a string containing the end of the input string. The second
    argument is the length of the returned string. If the input string
    is shorter than the length argument, returns null.
**************************************************************************/
extern "C" char *tail(char *input, int length)
{
char 	*atInput = 0;
int 	inputLength = (int)::strlen(input);
	if ( inputLength < length )
		return 0;
char 	*atTail = input + inputLength - length;
	atInput = (char*)::calloc(length + 1,sizeof(char));
	::strncpy(atInput,atTail,(size_t)(inputLength - length));
	return atInput;
}

/**************************************************************************
    Global toString routines (used in print statements)
**************************************************************************/
extern "C" char *toStringFromChar(char c)
{
char 	*line = 0;
	::asprintf(&line,"%c",c);
	return line;
}

extern "C" char *toStringFromDouble(double d)
{
char 	*line = 0;
	::asprintf(&line,"%g",d);
	return line;
}

extern "C" char *toStringFromFloat(float f)
{
char 	*line = 0;
	::asprintf(&line,"%g",f);
	return line;
}

extern "C" char *toStringFromInt(int i)
{
char 	*line = 0;
	::asprintf(&line,"%d",i);
	return line;
}

extern "C" char *toStringFromLong(long l)
{
char 	*line = 0;
	::asprintf(&line,"%ld",l);
	return line;
}

/**************************************************************************
    Return a truncated string for printing or copying into a buffer.
**************************************************************************/
extern "C" char *truncateString(char *text, int length)
{
char 	*truncated = (char*)::malloc(length + 1);
char 	*atTarget = truncated;
char 	*atText = text;
	while ( length-- > 0 )
		{
		*atTarget++ = *atText++;
		}
	*atTarget = '\0';
	return truncated;
}

/**************************************************************************
    URL decoder based on a method swiped off the internet
**************************************************************************/
extern "C" char *urlDecode(char *in)
{
char 	c = 0;
char 	v1 = 0;
char 	v2 = 0;
char 	tbl[] = {	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
char 	*out = (char*)::alloca(1 + ::strlen(in));
char 	*beg = out;
	if ( in )
		while ( c = *in++ )
			{
			if (c == '%')
				{
				if ( !(v1 = *in++) || (v1 = tbl[(unsigned char)v1]) < 0 || !(v2 = *in++) || (v2 = tbl[(unsigned char)v2]) < 0 )
					return 0;
				c = (v1 << 4) | v2;
				}
			*out++ = c;
			}
	*out = '\0';
	return beg;
}
int StringRoutines::debugIndent;
/*	Warning: the following methods were referenced but not declared
	read(int,char*,long)
*/
