#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "TapeSegment.h"
#include "Stak.h"
#include "DoubleLinkList.h"
#include "DoubleLink.h"
#include "Tape.h"

/*****************************************************************************
	Constructors
*****************************************************************************/
Tape::Tape(char *n, int l, int s)
{
	length = 0;
	reused = 0;
	segment = 0;
	list = 0;
	name = n;
	size = l;
	usedList = new Stak();
	stripSize = s / sizeof(void*);
	if ( s > stripSize * sizeof(void*) )
		stripSize++;
	blockLength = stripSize * sizeof(void*);
	//cout "Created " name,size:;
	zeroOut = 1;
	noRecycle = 0;
}

/*****************************************************************************
	Adds a new segment and sets segmentStart and segmentCurrent to point to the new segment.
    The list of segment strips is created the first time thru if it does not
    exist (note, when list is created here it uses the globalLinkTape to
    allocate links so adding a segment has to happen BEFORE the current list
    runs out of space).
*****************************************************************************/
void Tape::addSegment()
{
	if ( segment )
		size += size;
	segment = new TapeSegment(size,blockLength);
	// The list creation is deferred to avoid looping
	if ( !list )
		list = new DoubleLinkList();
	list->add((void*)segment);
	//cout "Adding tape segment", list.length, "to", name:`"new size", size:`"length",length:;
}

/*****************************************************************************
	Return a pointer to the nth entry on the tape
*****************************************************************************/
void **Tape::atIndex(int n)
{
DoubleLink 		*link = list->first;
TapeSegment 	*segmentage = (TapeSegment*)link->value;
int 			offset = 0;
int 			relative = 0;
void 			**spot = 0;
	while ( segmentage && n > segmentage->segmentLength + offset )
		{
		offset += segmentage->segmentLength;
		if ( link = link->next )
			segmentage = (TapeSegment*)link->value;
		else	segmentage = 0;
		}
	if ( segmentage )
		{
		relative = n - offset;
		spot = segmentage->segmentStart + (relative * stripSize);
		return spot;
		}
	return 0;
}

/*****************************************************************************
	Clear tape and free resources. Can safely delete the tape after clearing.
*****************************************************************************/
void Tape::clear()
{
	if ( !length )
		return;
	if ( list )
		{
		list->entry = 0;
		while ( segment = (TapeSegment*)list->next() )
			::free(segment->segmentStart);
		list->clear();
		}
	usedList->clear();
	segment = 0;
	length = size = stripSize = 0;
	delete list;
	delete usedList;
}

/*****************************************************************************
	Adds a strip passed in to the used list. Note Stak is extendable.
*****************************************************************************/
void Tape::free(void *spot)
{
	usedList->push(spot);
}

/*****************************************************************************
	Returns a strip of memory from tape and advances the segmentCurrent tape pointer.
	If you get to end of tape, a new tape segment is allocated. The strip
    returned is not zeroed out. That must be done after you get the strip.
*****************************************************************************/
void *Tape::getStrip()
{
void 	*spot = 0;
	if ( usedList->length && !noRecycle )
		{
		reused++;
		spot = usedList->pop();
		}
	else {
		if ( !segment )
			addSegment();
		spot = segment->next(stripSize);
		if ( !spot )
			::fprintf(stderr,"Tape getStrip: returned null strip\n");
		}
	length++;
	if ( segment->segmentLength + 5 == size )
		addSegment();
	return (void*)spot;
}

/*****************************************************************************
	Iterator to next thru allocated strips
*****************************************************************************/
void *Tape::next(void **spot)
{
TapeSegment 	*currentSegment = list->entry ? (TapeSegment*)list->entry->value : (TapeSegment*)list->next();
	if ( !spot )
		return (void*)(spot = currentSegment->segmentStart);
	spot += stripSize;
	if ( currentSegment->contains(spot) )
		return (void*)spot;
	if ( currentSegment = (TapeSegment*)list->next() )
		return (void*)(spot = currentSegment->segmentStart);
	return 0;
}

/*****************************************************************************
	Debugging routine to print out tape status;
*****************************************************************************/
void Tape::status()
{
int 	listLength = list ? list->length : 1;
	::printf("%s segments: %d\n\tlength %d\n\tsegment size %d\n\tsegment length %d\n\tstrip size %d\n\treused: %d\n\tOn used list: %d\n",name,listLength,length,size,segment->segmentLength,stripSize,reused,usedList->length);
}
