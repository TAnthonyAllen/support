class HashList;
/*******************************************************************************
	A class that contains a link item for a HashList. None of the methods
    pay any attention to the HashList hash (hashing is controled by HashList).
*******************************************************************************/

class HashLink
{
public:
char *key;
int linkHash;
void *value;
HashList *list;
HashLink *next;
HashLink *prior;
HashLink *nextHash;
HashLink(void *item);
void append(HashLink *link);
HashLink *append(void *item);
void appendToHash(HashLink *link);
void extract();
void insert(HashLink *toBeInserted);
HashLink *insert(char *name, void *item);
HashLink *insert(void *item);
void remove();
};
