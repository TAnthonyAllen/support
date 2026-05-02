#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "TapeSegment.h"

TapeSegment::TapeSegment(int size, int recordLength)
{
	segmentLength = 0;
	segmentStart = (void**)::calloc(size,recordLength);
	segmentCurrent = segmentStart;
	segmentSize = size;
}

/*****************************************************************************
	Returns true if the segment contains the spot passed in on a strip boundary.
*****************************************************************************/
int TapeSegment::contains(void **spot)
{
	return spot >= segmentStart && spot - segmentStart < segmentSize;
}

/*****************************************************************************
	Return the next strip in the segment.
*****************************************************************************/
void *TapeSegment::next(int stripSize)
{
void 	**spot = 0;
	if ( segmentSize > segmentLength )
		{
		segmentLength++;
		spot = segmentCurrent;
		segmentCurrent += stripSize;
		}
	return (void*)spot;
}
