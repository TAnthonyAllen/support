#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "StringRoutines.h"
#include "CharSet.h"
#include "Buffer.h"
#include "PLGitem.h"
#include "GroupRules.h"
#include "GroupControl.h"
#include "PLGparse.h"
#include "PLGset.h"
unsigned long *PLGset::mapper;

PLGset::PLGset()
{
	name = 0;
	specs = 0;
	map = 0;
	changed = 0;
	deferUpdate = 0;
	hidden = 0;
	negated = 0;
	parsed = 0;
	inSet = (unsigned char*)::calloc(256,sizeof(char));
	setBuffer = new Buffer("setBuffer",50);
}

PLGset::PLGset(char *input)
{
	name = 0;
	map = 0;
	changed = 0;
	deferUpdate = 0;
	hidden = 0;
	negated = 0;
	parsed = 0;
	inSet = (unsigned char*)::calloc(256,sizeof(char));
	setBuffer = new Buffer("setBuffer",50);
	specs = input;
	set(input);
}

/*****************************************************************************
    Clears the set
*****************************************************************************/
void PLGset::clear()
{
int 	i = 0;
	for ( i = 0; i < 256; i++ )
		inSet[i] = 0;
	hidden = 0;
	negated = 0;
	parsed = 0;
	setBuffer->reset();
}

/*****************************************************************************
    Check if a character is in the set
*****************************************************************************/
int PLGset::contains(char c)
{
	if ( c )
		return inSet[(unsigned char)c];
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
			if ( inSet[(unsigned char)*atText] )
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
	if ( text && !isEmpty() )
		{
		char 	*atText = text;
		while ( *atText )
			if ( inSet[(unsigned char)*atText] )
				return 1;
			else	atText++;
		}
	return 0;
}

/*****************************************************************************
    Check if the item contains any character in this set
*****************************************************************************/
int PLGset::foundIn(PLGitem *item)
{
int 	i = 0;
char 	*atText = item->itemStart;
	if ( item->itemStart && !isEmpty() )
		for ( i = item->itemLength; i > 0; i--, atText++ )
			if ( inSet[(unsigned char)*atText] )
				return 1;
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
	if ( name )
		{
		output->appendString("currentSet = ",0,0);
		output->appendString(name,0,0);
		output->appendString(";\n",0,0);
		}
	else
	if ( setBuffer->length() )
		{
		output->appendString("currentSet = getSet(\"",0,0);
		PLGset::printText(setBuffer->string(),output);
		output->appendString("\");\n",0,0);
		}
	output->mark = 0;
}

/*****************************************************************************
    Generate the code to create a named set
*****************************************************************************/
void PLGset::generateNamed(Buffer *output)
{
	output->setMark();
	if ( hidden )
		return;
	if ( !name || ::compare(name,"defaultSKIP") == 0 )
		return;
	output->appendString(name,0,0);
	output->appendString(" = getSet( \"",0,0);
	output->appendString(name,0,0);
	output->appendString("\",\"",0,0);
	PLGset::printText(setBuffer->string(),output);
	output->appendString("\" );\n",0,0);
}

/*****************************************************************************
    Stub. Retained for API compatibility with the older lazy-parse lineage.
    All construction parses immediately, so init() has nothing to do.
*****************************************************************************/
void PLGset::init()
{
}

/*****************************************************************************
    Returns true if the set is empty
*****************************************************************************/
int PLGset::isEmpty()
{
int 	i = 0;
	for ( i = 0; i < 256; i++ )
		if ( inSet[i] )
			return 0;
	return 1;
}

/*****************************************************************************
    Returns the number of chars in the set
*****************************************************************************/
int PLGset::length()
{
int 	i = 0;
int 	count = 0;
	for ( i = 0; i < 256; i++ )
		if ( inSet[i] )
			count++;
	return count;
}

/*****************************************************************************
    Append text in a quoted string representation to the buffer passed in.
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
				output->appendChar('\\',0,0);
				output->appendChar('n',0,0);
				break;
			case '\r':
				output->appendChar('\\',0,0);
				output->appendChar('r',0,0);
				break;
			case '\f':
				output->appendChar('\\',0,0);
				output->appendChar('f',0,0);
				break;
			case '\t':
				output->appendChar('\\',0,0);
				output->appendChar('t',0,0);
				break;
			case '"':
				output->appendChar('\\',0,0);
				output->appendChar('"',0,0);
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
						output->appendChar('\\',0,0);
						atText++;
						output->appendChar(*atText,0,0);
						continue;
					}
				output->appendChar('\\',0,0);
				output->appendChar('\\',0,0);
				break;
			default:
				output->appendChar(*atText,0,0);
			}
}

/*****************************************************************************
    Turn off a character in the set
*****************************************************************************/
void PLGset::reset(int a)
{
	if ( a >= 0 && a < 256 )
		inSet[a] = 0;
	if ( !deferUpdate && changed )
		updateBuffer();
}

/*****************************************************************************
    Turn off the contents of set in this set
*****************************************************************************/
void PLGset::reset(PLGset *set)
{
int 	i = 0;
	if ( isEmpty() )
		return;
	set->init();
	for ( i = 0; i < 256; i++ )
		if ( set->inSet[i] )
			inSet[i] = 0;
	if ( !deferUpdate && changed )
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
	if ( !deferUpdate && changed )
		updateBuffer();
}

/*****************************************************************************
    Add a character to the set
*****************************************************************************/
void PLGset::set(int a)
{
	if ( a >= 0 && a < 256 )
		{
		if ( inSet[a] )
			return;
		changed = 1;
		inSet[a] = 1;
		setBuffer->appendChar((char)a,0,0);
		if ( !deferUpdate && changed )
			updateBuffer();
		}
}

/*****************************************************************************
    Add every character in a string to the set, taking into account negation
    and ranges. Negation is resolved here: if input starts with '^', the
    set is built normally and then flipped at the end. The `negated` flag
    is set for informational purposes but no method reads it for matching.
*****************************************************************************/
void PLGset::set(char *text)
{
char 			*atText = text;
unsigned char 	lastChar = 0;
unsigned char 	setting = 0;
int 			i = 0;
int 			value = 0;
int 			negate = 0;
	deferUpdate = 1;
	if ( atText )
		{
		if ( *atText == '^' && !setBuffer->length() )
			{
			negate = 1;
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
		if ( negate )
			{
			for ( i = 0; i < 256; i++ )
				inSet[i] = !inSet[i];
			}
		}
	deferUpdate = 0;
	if ( !deferUpdate && changed )
		updateBuffer();
}

/*****************************************************************************
    Add a set to this set
*****************************************************************************/
void PLGset::set(PLGset *set)
{
int 	i = 0;
	set->init();
	for ( i = 0; i < 256; i++ )
		if ( set->inSet[i] )
			inSet[i] = 1;
	if ( !deferUpdate && changed )
		updateBuffer();
}

/*****************************************************************************
    Add a range of characters to the set
*****************************************************************************/
void PLGset::set(int left, int right)
{
char 	leftC = (char)left;
	if ( inSet[(unsigned char)leftC] )
		::fprintf(stderr,"PLGset: WARNING adding range but set already contains range start %c\n",leftC);
int 	i = 0;
int 	flag = deferUpdate;
	deferUpdate = 1;
	for ( i = left; i <= right; i++ )
		set(i);
	deferUpdate = flag;
	if ( !deferUpdate && changed )
		updateBuffer();
}

/*****************************************************************************
    Set the name if there is none to quoted representation of set text.
*****************************************************************************/
void PLGset::setName()
{
Buffer 	*buffer = new Buffer();
	buffer->appendString("[",0,0);
	PLGset::printText(setBuffer->string(),buffer);
	buffer->appendString("]",0,0);
	name = buffer->toString();
	delete buffer;
}

/*****************************************************************************
    Add every character in a string, ignoring negation and ranges
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
    Advance the parser pointer if the current character is in this set.
    Pointer-in / pointer-out variant — sister to skip(PLGparse).
*****************************************************************************/
char *PLGset::skip(char *plgStart)
{
	while ( plgStart && *plgStart )
		{
		if ( inSet[(unsigned char)*plgStart] )
			plgStart++;
		else	break;
		}
	return plgStart;
}

/*******************************************************************************
    Returns a string representation of the PLGset. Walks inSet[256] and emits
    each character that's present, with its decimal value for unambiguous
    debugging.
*******************************************************************************/
char *PLGset::toString()
{
int 	i = 0;
int 	count = 0;
char 	*output = 0;
char 	*text = setBuffer->toString();
Buffer 	*buffer = GroupControl::groupController->groupRules->stringBUFFER;
	if ( name )
		{
		buffer->appendString("Set: ",0,0);
		buffer->appendString(name,0,0);
		buffer->appendString("\n",0,0);
		}
	else	buffer->appendString("No name set:\n",0,0);
	if ( text )
		{
		buffer->appendString("\ttext: ",0,0);
		buffer->appendString(text,0,0);
		buffer->appendString("\n",0,0);
		}
	else	buffer->appendString("\tNo text\n",0,0);
	buffer->appendString("\tcontents:\n",0,0);
	for ( i = 0; i < 256; i++ )
		if ( inSet[i] )
			{
			count++;
			buffer->appendString("\t  [",0,0);
			buffer->appendInt(i,0,0);
			buffer->appendString("] ",0,0);
			buffer->appendChar((char)i,0,0);
			buffer->appendString("\n",0,0);
			}
	buffer->appendString("\ttotal ",0,0);
	buffer->appendInt(count,0,0);
	buffer->appendString("\n",0,0);
	output = buffer->toString();
	return output;
}

/*******************************************************************************
    Update setBuffer to keep it current after the set has been changed by
    non-string operations (set(int), reset(int), etc.). Re-derives the
    setBuffer content from inSet[]. Escapes whitespace.
*******************************************************************************/
void PLGset::updateBuffer()
{
int 	i = 0;
	setBuffer->reset();
	if ( isEmpty() )
		return;
	for ( i = 0; i < 256; i++ )
		if ( inSet[i] )
			{
			if ( i >= 32 && i <= 126 )
				setBuffer->appendChar((char)i,0,0);
			else
			if ( i == 9 )
				setBuffer->appendString("\\t",0,0);
			else
			if ( i == 10 )
				setBuffer->appendString("\\n",0,0);
			else
			if ( i == 12 )
				setBuffer->appendString("\\f",0,0);
			else
			if ( i == 13 )
				setBuffer->appendString("\\r",0,0);
			}
	changed = 0;
}
