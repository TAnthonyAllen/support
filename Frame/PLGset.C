#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "StringRoutines.h"
#include "Buffer.h"
#include "Segment.h"
#include "PLGset.h"
unsigned long *PLGset::mapper;

PLGset::PLGset()
{
	name = 0;
	specs = 0;
	deferUpdate = 0;
	hidden = 0;
	negated = 0;
	parsed = 0;
	setUpdated = 0;
	map = (unsigned long*)::calloc(4,sizeof(long));
	if ( !PLGset::mapper )
		PLGset::mapper = (unsigned long*)::calloc(257,sizeof(long));
	setBuffer = ::bufferFactory3("setBuffer",50);
}

PLGset::PLGset(char *input)
{
	name = 0;
	deferUpdate = 0;
	hidden = 0;
	negated = 0;
	parsed = 0;
	setUpdated = 0;
	map = (unsigned long*)::calloc(4,sizeof(long));
	if ( !PLGset::mapper )
		PLGset::mapper = (unsigned long*)::calloc(257,sizeof(long));
	setBuffer = ::bufferFactory3("setBuffer",50);
	specs = input;
	set(input);
}

/*****************************************************************************
	Clears the set
*****************************************************************************/
void PLGset::clear()
{
	map[0] = 0;
	map[1] = 0;
	map[2] = 0;
	map[3] = 0;
	hidden = negated = parsed = 0;
	setBuffer->reset();
}

/*****************************************************************************
	Check if a character is in the set
*****************************************************************************/
int PLGset::contains(char c)
{
	if ( c )
		{
		unsigned char 	a = c;
		int 			word = a / 64;
		int 			bit = a % 64;
		unsigned long 	foundIt = 0;
		unsigned long 	mask = Segment::bitMask((int)a);
		//cout "contains: " (char)a,(int)a:this;
		if ( !bit )
			word--;
		foundIt = map[word] & mask;
		if ( negated )
			foundIt = !foundIt;
		return bit = foundIt > 0;
		}
	return 0;
}

/*****************************************************************************
	Check if every character in the string passed in is in the set
*****************************************************************************/
int PLGset::contains(char *s)
{
	if ( s )
		{
		char 	*atText = s;
		while ( *atText )
			if ( contains(*atText) )
				atText++;
			else	break;
		if ( *atText )
			return 0;
		return 1;
		}
	return 0;
}

/*****************************************************************************
	Check if the string passed in has any character in this set
*****************************************************************************/
int PLGset::foundIn(char *text)
{
	if ( text )
		{
		char 	*atText = text;
		if ( text && !isEmpty() )
			while ( *atText )
				if ( contains(*atText) )
					return 1;
				else	atText++;
		}
	return 0;
}

/*****************************************************************************
	Generate the code to implement this set
*****************************************************************************/
void PLGset::generate(Buffer *output)
{
	output->setMark();
	if ( hidden )
		return;
	if ( setBuffer->length() )
		{
		output->appendString("currentSet = getSet(");
		if ( name )
			{
			output->appendString("\"");
			output->appendString(name);
			output->appendString("\"");
			}
		if ( specs )
			{
			if ( name )
				output->appendString(",");
			output->appendString("\"");
			output->appendString(specs);
			output->appendString("\"");
			}
		else	printText(setBuffer->string(),output);
		output->appendString(");\n");
		}
	else	::fprintf(stderr,"PLGset: tried to print an empty set\n");
	output->mark = 0;
}

/*****************************************************************************
	Returns true if the set is empty
*****************************************************************************/
int PLGset::isEmpty()
{
	if ( map[0] + map[1] + map[2] + map[3] == 0 )
		return 1;
	return 0;
}

/*****************************************************************************
	Returns the number of bits set
*****************************************************************************/
int PLGset::length()
{
int 	count = ::bitCount(map[0]) + ::bitCount(map[1]) + ::bitCount(map[2]) + ::bitCount(map[3]);
	return count;
}

/*****************************************************************************
	append text in a quoted string representation to the buffer passed in.
*****************************************************************************/
void PLGset::printText(char *text, Buffer *output)
{
char 	*atText = 0;
	if ( !text || !output )
		return;
	for ( atText = text; *atText; atText++ )
		switch (*atText)
			{
			case '\n':
				output->appendChar('\\');
				output->appendChar('n');
				break;
			case '\r':
				output->appendChar('\\');
				output->appendChar('r');
				break;
			case '\f':
				output->appendChar('\\');
				output->appendChar('f');
				break;
			case '\t':
				output->appendChar('\\');
				output->appendChar('t');
				break;
			case '"':
				output->appendChar('\\');
				output->appendChar('"');
				break;
			case '\\':
				switch ( *(atText + 1) )
					{
					case 'n':
					case 'r':
					case 'f':
					case 't':
					case '"':
					case '\\':
						output->appendChar('\\');
						atText++;
						output->appendChar(*atText);
						continue;
					}
				output->appendChar('\\');
				output->appendChar('\\');
				break;
			default:
				output->appendChar(*atText);
			}
}

/*****************************************************************************
	Turn off a character in the set
*****************************************************************************/
void PLGset::reset(int a)
{
unsigned long 	mask = 0;
int 			word = a / 64;
int 			bit = a % 64;
	if ( bit )
		mask = 1LL << 64 - bit;
	else	mask = 1LL;
	map[word] &= ~mask;
	if ( !deferUpdate )
		updateBuffer();
}

/*****************************************************************************
	Turn off the contents of set in this set
*****************************************************************************/
void PLGset::reset(PLGset *set)
{
	if ( isEmpty() )
		return;
	if ( set->negated )
		{
		map[0] &= set->map[0];
		map[1] &= set->map[1];
		map[2] &= set->map[2];
		map[3] &= set->map[3];
		}
	else {
		map[0] &= ~set->map[0];
		map[1] &= ~set->map[1];
		map[2] &= ~set->map[2];
		map[3] &= ~set->map[3];
		}
	if ( !deferUpdate )
		updateBuffer();
}

/*****************************************************************************
	Turn off a range of characters in the set
*****************************************************************************/
void PLGset::reset(int left, int right)
{
int 	i = 0;
int 	flag = deferUpdate;
	deferUpdate = 1;
	for ( i = left; i <= right; i++ )
		reset(i);
	deferUpdate = flag;
	if ( !deferUpdate )
		updateBuffer();
}

/*****************************************************************************
	Add a character to the set
*****************************************************************************/
void PLGset::set(int a)
{
char 	c = (char)a;
	if ( ::containsCharacter(setBuffer->start,c) )
		return;
int 	word = a / 64;
int 	bit = a % 64;
int 	offset = 64 - bit;
unsigned long result = 0;
	if ( bit )
		map[word] |= 1LL << offset;
	else	map[--word] |= 1LL;
	result = *(map + word);
	setBuffer->appendChar(c);
	if ( !deferUpdate )
		updateBuffer();
}

/*****************************************************************************
	Add every character in a string to the set taking into account negation
    and ranges
*****************************************************************************/
void PLGset::set(char *text)
{
char 			*atText = text;
unsigned char 	lastChar = 0;
unsigned char 	setting = 0;
int 			value = 0;
	deferUpdate = 1;
	if ( atText )
		{
		if ( *atText == '^' && !setBuffer->length() )
			{
			negated = 1;
			atText++;
			}
		while ( *atText )
			{
			if ( *atText == '\\' )
				{
				atText++;
				switch (*atText)
					{
					case 'n':
						value = '\n';
						break;
					case 't':
						value = '\t';
						break;
					case 'r':
						value = '\r';
						break;
					default:
						value = *atText;
					}
				set(value);
				}
			else {
				if ( *atText == '-' )
					{
					if ( lastChar )
						{
						if ( setting = (unsigned char)*(atText + 1) )
							{
							set((int)lastChar,(int)setting);
							atText++;
							}
						atText++;
						lastChar = 0;
						continue;
						}
					}
				if ( lastChar )
					set((int)lastChar);
				lastChar = (unsigned char)*atText;
				}
			atText++;
			}
		if ( lastChar )
			set((int)lastChar);
		}
	deferUpdate = 0;
}

/*****************************************************************************
	Add a set to this set
*****************************************************************************/
void PLGset::set(PLGset *set)
{
	if ( isEmpty() )
		{
		map[0] = set->map[0];
		map[1] = set->map[1];
		map[2] = set->map[2];
		map[3] = set->map[3];
		if ( set->negated )
			negated = 1;
		}
	else
	if ( set->negated )
		{
		map[0] |= ~set->map[0];
		map[1] |= ~set->map[1];
		map[2] |= ~set->map[2];
		map[3] |= ~set->map[3];
		}
	else {
		map[0] |= set->map[0];
		map[1] |= set->map[1];
		map[2] |= set->map[2];
		map[3] |= set->map[3];
		}
	if ( !deferUpdate )
		updateBuffer();
}

/*****************************************************************************
	Add a range of characters to the set
*****************************************************************************/
void PLGset::set(int left, int right)
{
char 	leftC = (char)left;
	if ( ::containsCharacter(setBuffer->start,leftC) )
		::fprintf(stderr,"PLGset: WARNING adding range but set already contains range start %c\n",leftC);
int 	i = 0;
int 	flag = deferUpdate;
	deferUpdate = 1;
	for ( i = left; i <= right; i++ )
		set(i);
	deferUpdate = flag;
	if ( !deferUpdate )
		updateBuffer();
}

/*****************************************************************************
	Add every character in a string ignoring negation and ranges
*****************************************************************************/
void PLGset::setSimple(char *text)
{
char 	*atText = text;
	deferUpdate = 1;
	if ( atText )
		while ( *atText )
			set((int)*atText++);
	deferUpdate = 0;
}

/*****************************************************************************
	Advance the parser pointer if the current character is in this set
    You pass in the parser pointer not the parser
*****************************************************************************/
char *PLGset::skip(char *plgStart)
{
int 			bit = 0;
int 			foundIt = 0;
int 			word = 0;
unsigned char 	a = 0;
	while ( plgStart && *plgStart )
		{
		a = *plgStart;
		word = a / 64;
		bit = a % 64;
		foundIt = 0;
		if ( bit )
			{
			if ( map[word] & 1LL << 64 - bit )
				foundIt = 1;
			}
		else	foundIt = map[word] & 1LL;
		if ( foundIt || negated )
			plgStart++;
		else	break;
		}
	return plgStart;
}

/*******************************************************************************
	returns a string representation of the PLGset
*******************************************************************************/
char *PLGset::toString()
{
int 		i = 1;
int 		j = 0;
int 		k = 0;
int 		count = 0;
int 		limit = sizeof(long) * 8;
Segment 	*segment = new Segment(0);
char 		*output = 0;
char 		*text = setBuffer->toString();
	setBuffer->reset();
	if ( name )
		{
		setBuffer->appendString("Set: ");
		setBuffer->appendString(name);
		setBuffer->appendString("\n");
		}
	else	setBuffer->appendString("No name set:\n");
	if ( text )
		{
		setBuffer->appendString("\ttext: ");
		printText(text,setBuffer);
		setBuffer->appendString("\n");
		}
	else {
		setBuffer->appendString("\t");
		setBuffer->appendString("No text: ");
		}
	setBuffer->appendString("\t");
	setBuffer->appendString("contents");
	setBuffer->appendString("\n");
	setBuffer->appendString("\t");
	while ( j < 4 )
		{
		for ( i = 1; i <= limit; i++ )
			{
			count++;
			if ( count < 32 )
				continue;
			if ( count > 126 )
				break;
			k++;
			if ( map[j] & Segment::bitMask(i) || negated )
				setBuffer->appendChar((char)count);
			}
		j++;
		}
	setBuffer->appendString("\n");
	setBuffer->appendString("\t");
	setBuffer->appendString("total");
	setBuffer->appendString(" ");
	setBuffer->appendCount(k);
	setBuffer->appendString("\n");
	output = setBuffer->toString();
	delete segment;
	setBuffer->reset();
	setBuffer->appendString(text);
	return output;
}

/*******************************************************************************
	Updates the setBuffer to keep it current after setting or resetting
*******************************************************************************/
void PLGset::updateBuffer()
{
int 			i = 1;
int 			j = 0;
int 			count = 0;
int 			limit = sizeof(long) * 8;
unsigned long 	mapped = 0;
unsigned long 	masked = 0;
	setBuffer->reset();
	if ( map[0] || map[1] || map[2] || map[3] )
		{
		if ( negated )
			setBuffer->appendChar('^');
		while ( j < 4 )
			{
			mapped = map[j];
			for ( i = 1; i <= limit; i++ )
				{
				char 	c = (char)++count;
				masked = Segment::bitMask(count);
				if ( count >= 32 && count <= 126 )
					if ( mapped & masked )
						setBuffer->appendChar(c);
					else
					if ( count == 9 )
						setBuffer->appendString("\\t");
					else
					if ( count == 10 )
						setBuffer->appendString("\\n");
					else
					if ( count == 12 )
						setBuffer->appendString("\\f");
					else
					if ( count == 13 )
						setBuffer->appendString("\\r");
				}
			j++;
			}
		}
	setUpdated = 1;
}
/*	Warning: the following methods were referenced but not declared
	isEmpty()
	printText(char*,Buffer*)
*/
