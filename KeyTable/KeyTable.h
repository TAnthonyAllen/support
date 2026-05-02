class DoubleLinkList;
class KeyTableItem;
class PLGset;
class DoubleLink;
/*******************************************************************************
	A class that contains a trinary tree for indexing key fields
	Note that the 0th entry in a table is always the missing value
	in other words if the table does not contain an entry any lookup
	using find will return a pointer to the first entry which should
	be entered as missing
*******************************************************************************/

class KeyTable
{
public:
char *name;
DoubleLinkList *letterMaps;
int debug;
int max;
int size;
KeyTableItem *current;
KeyTableItem *first;
KeyTableItem *last;
void **keyValueList;
PLGset *keySet;
KeyTable();
KeyTable(char *n);
int add(char *key);
int add(char *key, void *pointer);
void dump(int flag);
void *find(char *key);
KeyTableItem *findKeyItem(char *key);
KeyTableItem *getNext();
void *itemAt(int index);
KeyTableItem *matches(char *key);
DoubleLink *newLink();
void *next();
};
