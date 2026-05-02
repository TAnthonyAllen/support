class Stak;
class TapeSegment;
class DoubleLinkList;
/*****************************************************************************
	A simple memory allocator that manages an indexed block of memory aligned
	on int boundaries.
*****************************************************************************/

class Tape
{
public:
char *name;
Stak *usedList;
int blockLength;
int length;
int reused;
int size;
int stripSize;
TapeSegment *segment;
DoubleLinkList *list;
struct 
	{
	unsigned int noRecycle:1;
	unsigned int zeroOut:1;
	};
Tape(char *n, int l, int s);
void addSegment();
void **atIndex(int n);
void clear();
void free(void *spot);
void *getStrip();
void *next(void *spot);
void *next(void **spot);
void status();
};
