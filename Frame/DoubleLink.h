class DoubleLinkList;
/*******************************************************************************
	A class that contains a double link item
*******************************************************************************/

class DoubleLink
{
public:
char *key;
int linkHash;
int linkOrder;
void *value;
DoubleLinkList *list;
DoubleLink *next;
DoubleLink *prior;
DoubleLink(void* item);
void append(DoubleLink *link);
DoubleLink *append(void* item);
void extract();
void insert(DoubleLink *toBeInserted);
DoubleLink *insert(char *name, void* item);
DoubleLink *insert(void* item);
void remove();
};
