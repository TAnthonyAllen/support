class DoubleLink;
class DoubleLinkList;
/******************************************************************************
	BaseHash provides an ordered hash. Altho there are push and pop aliases
    a BaseHash is not a stack because items that share the same hash are
    kept together, not in the order they were entered.
******************************************************************************/

class BaseHash
{
public:
int hashSize;
DoubleLink **hashStack;
DoubleLinkList *hashList;
BaseHash();
BaseHash(int tableSize);
DoubleLink *add(char *name, void *entry);
DoubleLink *add(char *name);
void clear();
DoubleLink *find(char *name);
void *get(char *name);
int getHashIndex(char *name);
void listKeys();
void *next(char *name);
DoubleLink *put(char *name, void *entry);
DoubleLink *put(char *name);
void *remove(char *name);
void removeFromHash(DoubleLink *atHash);
};
