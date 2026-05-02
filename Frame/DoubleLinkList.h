class Tape;
class DoubleLink;
/*******************************************************************************
	Doubly linked list class
*******************************************************************************/

class DoubleLinkList
{
public:
static Tape *globalLinkTape;
DoubleLink *entry;
DoubleLink *first;
DoubleLink *last;
int length;
struct 
	{
	unsigned int hasKeys:1;
	unsigned int isSorted:3;
	unsigned int noDuplicates:1;
	};
#define sortAscending(button) (button == 1)
#define sortDescending(button) (button == 2)
DoubleLinkList();
DoubleLink *add(void* item);
DoubleLink *add(char *name);
DoubleLink *add(char *n, void* i);
DoubleLink *add(DoubleLink *link);
void addInLinkOrder(DoubleLink *inputLink);
void clear();
void *dQ();
DoubleLink *find(char *name);
void *get(char *name);
DoubleLink *getLink(void* item);
DoubleLink *getLink(char *n, void* item);
DoubleLink *insert(void* item);
DoubleLink *insert(char *name, void* item);
void *next();
void *next(DoubleLink *&link);
DoubleLink *nextLink();
void *pop();
void *prior();
void *prior(DoubleLink *&link);
DoubleLink *priorLink();
DoubleLink *put(char *n, void* i);
DoubleLink *put(char *n);
int remove(void* item);
void resetIterator();
void sort(int (*comparisor)(DoubleLink *, DoubleLink *));
void **toArray();
};
