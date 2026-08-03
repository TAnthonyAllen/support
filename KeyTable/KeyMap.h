class CharSet;
class DoubleLinkList;
class KeyTableItem;
/*******************************************************************************
	A class that contains a map of follow letters
*******************************************************************************/

class KeyMap
{
public:
CharSet *follow;
DoubleLinkList *entries;
int length;
char letter;
KeyMap(char l);
KeyTableItem *find(char *key);
};
