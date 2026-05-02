#include <string.h>
#include <stdio.h>
#include "DoubleLinkList.h"
#include "DoubleLink.h"
#include "SearchItem.h"
#include "SearchNode.h"
#include "SearchTree.h"

SearchTree::SearchTree()
{
	root = 0;
}

/*******************************************************************************
	This version of add stores a string keyed by the string
*******************************************************************************/
void SearchTree::add(char *name)
{
	add(name,(void*)name);
}

/*******************************************************************************
	Insert an entry into the search tree. This algorithm can be speeded up
	by replacing SearchNode.new with a more efficient buffer allocation
*******************************************************************************/
void SearchTree::add(char *name, void *entry)
{
char 		*n = name;
int 		compared = 0;
SearchItem 	*item = new SearchItem(entry);
SearchNode 	*last = 0;
SearchNode 	*node = 0;
	if ( !root )
		root = new SearchNode(name);
	node = root;
	while ( 1 )
		{
		compared = *n - node->splitter;
		last = node;
		if ( !compared )
			{
			if ( !*++n )
				{
				node->append(item);
				return;
				}
			node = node->equal;
			if ( !node )
				{
				node = new SearchNode(n++);
				last->equal = node;
				break;
				}
			}
		else
		if ( compared < 0 )
			{
			node = node->left;
			if ( !node )
				{
				node = new SearchNode(n++);
				last->left = node;
				break;
				}
			}
		else {
			node = node->right;
			if ( !node )
				{
				node = new SearchNode(n++);
				last->right = node;
				break;
				}
			}
		}
	while ( 1 )
		{
		last = node;
		if ( !*n )
			{
			node->append(item);
			return;
			}
		node = new SearchNode(n++);
		last->equal = node;
		}
}

/*******************************************************************************
	Find the search node that matches the character pointer argument
*******************************************************************************/
void *SearchTree::find(char *name)
{
SearchItem 	*item = 0;
	if ( name && (item = search(name)) )
		return item->value;
	return 0;
}

/*******************************************************************************
	Walk the tree and return a sorted doubly linked list of its contents
*******************************************************************************/
DoubleLinkList *SearchTree::getList()
{
SearchItem 		*item = 0;
SearchNode 		*node = root;
DoubleLinkList 	*list = new DoubleLinkList();
	while ( node )
		{
		/*
		cout splitter;
		if left
		cout " left: " left.splitter;
		if right
		cout " right: " right.splitter;
		cout :;
		*/
		if ( node->left )
			{
			if ( node->right )
				list->add((void*)node->right);
			if ( node->equal )
				list->add((void*)node->equal);
			node = node->left;
			continue;
			}
		else {
			for ( item = node->start; item; item = item->next )
				list->add(item->value);
			if ( node->equal )
				{
				if ( node->right )
					list->add((void*)node->right);
				node = node->equal;
				continue;
				}
			else
			if ( node->right )
				{
				node = node->right;
				continue;
				}
			//node = (SearchNode)pop();
			::printf("SearchTree getList: not sure what happens here. Popped to\n");
			}
		}
	return list;
}

/*******************************************************************************
	Remove the first search item that matches the character pointer argument
	If there is more than one matching item, they are all removed from
	the SearchTree. The tree is not pruned of nodes, however, just the
	contents of the matching node are removed.
*******************************************************************************/
SearchItem *SearchTree::remove(char *atChar)
{
SearchNode 	*node = root;
	if ( atChar )
		while ( node )
			{
			if ( *atChar < node->splitter )
				node = node->left;
			else
			if ( *atChar == node->splitter )
				{
				atChar++;
				if ( !*atChar )
					{
					SearchItem 	*item = node->start;
					node->start = node->end = 0;
					return item;
					}
				node = node->equal;
				}
			else	node = node->right;
			}
	return 0;
}

/*******************************************************************************
	Find the first search item that matches the character pointer argument
	If there is more than one matching item, you can walk the item chain
	to get them all
*******************************************************************************/
SearchItem *SearchTree::search(char *atChar)
{
SearchNode 	*node = root;
	if ( atChar )
		while ( node )
			{
			if ( *atChar < node->splitter )
				node = node->left;
			else
			if ( *atChar == node->splitter )
				{
				atChar++;
				if ( !*atChar )
					return node->start;
				node = node->equal;
				}
			else	node = node->right;
			}
	return 0;
}
/*	Warning: the following methods were referenced but not declared
	append(SearchItem*)
*/
