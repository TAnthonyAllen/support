class SearchItem;
/*******************************************************************************
	SearchNode represents a node in a SearchTree.  It is implemented as a
        doubly linked list to facilitate bidirectional traversals and guarantee
        the order in cases where there are duplicates.
        
        At some point need to implement delete(). This is done in the SearchTree
        remove() method.
*******************************************************************************/

class SearchNode
{
public:
SearchNode *equal;
SearchNode *left;
SearchNode *right;
char splitter;
SearchItem *start;
SearchItem *end;
SearchNode(char *c);
void append(SearchItem *item);
};
