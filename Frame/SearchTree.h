class SearchNode;
class DoubleLinkList;
class SearchItem;
/*******************************************************************************
	SearchTree class implements a ternary search tree that stores a void*
	pointer to some value
*******************************************************************************/

class SearchTree
{
public:
SearchNode *root;
SearchTree();
void add(char *name);
void add(char *name, void *entry);
void *find(char *name);
DoubleLinkList *getList();
SearchItem *remove(char *atChar);
SearchItem *search(char *atChar);
};
