/*******************************************************************************
	SearchItem represents a value associated with a SearchNode.
*******************************************************************************/

class SearchItem
{
public:
void *value;
SearchItem *next;
SearchItem(void *entry);
};
