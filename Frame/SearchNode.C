#include <string.h>
#include <stdio.h>
#include "SearchItem.h"
#include "SearchNode.h"

SearchNode::SearchNode(char *c)
{
	left = 0;
	right = 0;
	end = 0;
	splitter = *c;
	start = end = 0;
	equal = left = right = 0;
}

void SearchNode::append(SearchItem *item)
{
	if ( !start )
		{
		end = start = item;
		return;
		}
	end->next = item;
	end = item;
}
