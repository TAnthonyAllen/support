class HashLink;
/******************************************************************************
	Hasher provides a hashed collection. To add a Hasher to GroupItem, will have
    to modify every add,insert,extract,push,pop, and remove method in GroupItem.
    Will also have to modify any find or get method that returns a GroupItem
******************************************************************************/

class Hasher
{
public:
int hashSize;
HashLink **hashStack;
Hasher();
Hasher(int tableSize);
void addHash(HashLink *newLink);
void clear();
HashLink *find(char *name);
int getHashIndex(char *name);
void remove(HashLink *link);
};
