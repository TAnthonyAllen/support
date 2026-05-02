/******************************************************************************
	The BaseEntry defines what gets stored. It contains pointers into two
    linked lists
		the hash list, which is a list of entries with the same hash
		the order list, which is ordered to support LIFO or FIFO
	BaseEntry is modelled after the DoubleLink class
******************************************************************************/

class BaseEntry
{
public:
char *key;
void *value;
BaseEntry *next;
BaseEntry *nextHash;
BaseEntry *prior;
BaseEntry *priorHash;
BaseEntry();
BaseEntry(char *k, void *item);
BaseEntry *append(BaseEntry *entry);
void remove();
};
