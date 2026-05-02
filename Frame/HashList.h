class HashLink;
class Hasher;
class Tape;
/*******************************************************************************
	Doubly linked list class that supports hashing. Because of the use of Avoid,
    this class source code is intended to be included after an external declaration
    that defines Avoid as an alias for the value in a HashLink.
*******************************************************************************/

class HashList
{
public:
HashLink *entry;
HashLink *first;
HashLink *last;
int hashListLength;
Hasher *hasher;
struct 
	{
	unsigned int hasKeys:1;
	unsigned int isSorted:3;
	unsigned int noDuplicates:1;
	};
#define sortAscending(button) (button == 1)
#define sortDescending(button) (button == 2)
static Tape *globalHashTape;
HashList();
HashList(Hasher *h);
HashLink *add(void *item);
HashLink *add(HashLink *link);
HashLink *add(char *n, void *i);
void clear();
void *dQ();
HashLink *find(char *name);
void *get(char *name);
HashLink *getLink(void *item);
HashLink *getLink(char *n, void *item);
HashLink *insert(void *item);
HashLink *insert(char *name, void *item);
void *next();
void *next(HashLink *&link);
HashLink *nextLink();
void *pop();
void *prior();
void *prior(HashLink *&link);
HashLink *priorLink();
HashLink *put(char *n, void *i);
HashLink *put(char *n);
int remove(void *item);
void resetIterator();
void sort(int (*comparisor)(HashLink *, HashLink *));
void **toArray();
};
