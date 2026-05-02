/*****************************************************************************
	A class wrapper for Tape segments.
*****************************************************************************/

class TapeSegment
{
public:
int segmentLength;
int segmentSize;
void **segmentStart;
void **segmentCurrent;
TapeSegment(int size, int recordLength);
int contains(void **spot);
void *next(int stripSize);
};
