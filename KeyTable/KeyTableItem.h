/*******************************************************************************
	A class that contains an entry in a KeyTable
*******************************************************************************/

class KeyTableItem
{
public:
char *key;
union 
	{
	void *value;
	char *itemText;
	};
int position;
KeyTableItem *next;
KeyTableItem(char *k, void *v);
};
