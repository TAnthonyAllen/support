#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Stak.h"

/*****************************************************************************
	Constructors
*****************************************************************************/
Stak::Stak()
{
	start = 0;
	entry = 0;
	length = 0;
	size = 100;
	fifo = start = (void**)::calloc(size,sizeof(void*));
	end = start;
}

Stak::Stak(int s)
{
	start = 0;
	entry = 0;
	length = 0;
	size = s;
	fifo = start = (void**)::calloc(size,sizeof(void*));
	end = start;
}

/*****************************************************************************
	clear the stack
*****************************************************************************/
void Stak::clear()
{
	length = 0;
	fifo = end = start;
	*end = (void*)0;
}

/*****************************************************************************
	Iterates thru the stack in fifo order returning the next entry.
*****************************************************************************/
void *Stak::next()
{
	if ( !length )
		return entry = 0;
	if ( !entry || entry < start )
		entry = start;
	else	entry++;
	if ( entry >= end )
		return entry = 0;
	return *entry;
}

void *Stak::pop()
{
void 	*top = 0;
	if ( !length )
		return 0;
	length--;
	end--;
	top = *end;
	*end = (void*)0;
	if ( !length )
		entry = 0;
	return top;
}

/*****************************************************************************
	popOff clears multiple entries off of the stack. It does not complain if
    you try to clear off more entries than the stack contains.
*****************************************************************************/
void Stak::popOff(int i)
{
	if ( length < i )
		{
		end = start;
		length = 0;
		}
	else {
		length -= i;
		end -= i;
		}
	*end = (void*)0;
	if ( !length )
		entry = 0;
}

/*****************************************************************************
	Iterates thru the stack in lifo order returning the next entry.
*****************************************************************************/
void *Stak::prior()
{
	if ( !length )
		return entry = 0;
	if ( !entry || entry >= end )
		entry = end - 1;
	else	entry--;
	if ( entry < start )
		return entry = 0;
	return *entry;
}

/*****************************************************************************
	Push and pop methods just move the top pointer and set length, resizing
	when length reaches size
*****************************************************************************/
void Stak::push(void *v)
{
	if ( length == size )
		resize();
	*end++ = v;
	length++;
}

/*****************************************************************************
	Resize doubles the size and does a realloc
*****************************************************************************/
void Stak::resize()
{
long 	offset = fifo - start;
	size = size * 2;
	start = (void**)::reallocf((void*)start,size * sizeof(void*));
	fifo = start + offset;
	end = start + length;
	*end = (void*)0;
	entry = 0;
}

/*****************************************************************************
	return whatever is on the top of the stack without popping
*****************************************************************************/
void *Stak::top()
{
	if ( !length )
		return 0;
	return *(end - 1);
}
