#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "StringRoutines.h"
#include "Tape.h"
#include "HashLink.h"
#include "Hasher.h"
#include "HashList.h"
Tape *HashList::globalHashTape;

/*****************************************************************************
	Constructors
*****************************************************************************/
HashList::HashList()
{
	entry = 0;
	first = 0;
	last = 0;
	hashListLength = 0;
	hasher = 0;
	hasKeys = 0;
	isSorted = 0;
	noDuplicates = 0;
	if ( !HashList::globalHashTape )
		HashList::globalHashTape = new Tape("globalHashTape",50000,sizeof(HashLink));
}

HashList::HashList(Hasher *h)
{
	entry = 0;
	first = 0;
	last = 0;
	hashListLength = 0;
	hasKeys = 0;
	isSorted = 0;
	noDuplicates = 0;
	if ( !HashList::globalHashTape )
		HashList::globalHashTape = new Tape("globalHashTape",50000,sizeof(HashLink));
	hasher = h;
}

/*****************************************************************************
	Add to the list
*****************************************************************************/
HashLink *HashList::add(void *item)
{
	//if hasKeys cerr "HashList add: no key supplied to hasKey list":;
	if ( !first )
		{
		first = last = getLink(item);
		last->list = this;
		hashListLength++;
		}
	else	last = last->append(item);
	return last;
}

HashLink *HashList::add(HashLink *link)
{
	//cout "HashList: adding link",link.key:;
	if ( link )
		{
		if ( !first )
			{
			first = last = link;
			}
		else {
			last->next = link;
			link->prior = last;
			link->next = 0;
			last = link;
			}
		link->list = this;
		hashListLength++;
		}
	else	::fprintf(stderr,"HashList add(link): no link passed in\n");
	return link;
}

/*****************************************************************************
	Add to the keyed list. If list is not sorted it just appends. In any
    case the link key is set.
*****************************************************************************/
HashLink *HashList::add(char *n, void *i)
{
HashLink 	*link = 0;
	hasKeys = 1;
	if ( isSorted )
		{
		int 	flip = sortDescending(isSorted) ? -1 : 1;
		entry = 0;
		while ( link = nextLink() )
			{
			int 	comparison = flip * ::compare(n,link->key);
			if ( comparison > 0 )
				if ( link->next )
					continue;
				else	goto appendLink;
			else
			if ( comparison == 0 )
				goto appendLink;
			else	goto insertLink;
			}
		}
	if ( !link )
		{
		link = getLink(i);
		link->key = n;
		add(link);
		goto finishAdd;
		}
appendLink:
	link = link->append(i);
	link->key = n;
	goto finishAdd;
insertLink:
	link = link->insert(i);
	link->key = n;
finishAdd:
	if ( hasher )
		hasher->addHash(link);
	return link;
}

/*****************************************************************************
	Clear the list and release its resources (for reuse if globalHashTape is set)
*****************************************************************************/
void HashList::clear()
{
	if ( hasher )
		hasher->clear();
	for ( entry = first; entry; entry = entry->next )
		if ( HashList::globalHashTape )
			HashList::globalHashTape->free((void*)entry);
		else
		delete entry;
	first = entry = last = 0;
	hashListLength = 0;
}

/***************************************************************************
	Like pop treats the list as stack but pops off first item, not last.
***************************************************************************/
void *HashList::dQ()
{
void 	*stuff = (void*)0;
	if ( first )
		{
		stuff = first->value;
		first->remove();
		}
	return stuff;
}

/***************************************************************************
	If it exists, return the first link with key == name. The search takes
    advantage of sort status. If not sorted, examines sequentially. It
    leaves current entry as is.
***************************************************************************/
HashLink *HashList::find(char *name)
{
HashLink 	*saveEntry = entry;
HashLink 	*link = 0;
	if ( hasher )
		link = hasher->find(name);
	else
	if ( hasKeys && name )
		{
		entry = 0;
		if ( sortAscending(isSorted) )
			{
			while ( link = nextLink() )
				if ( ::compare(link->key,name) < 0 )
					continue;
				else
				if ( ::compare(link->key,name) == 0 )
					goto endFind;
				else	break;
			link = 0;
			}
		else
		if ( sortDescending(isSorted) )
			{
			while ( link = priorLink() )
				if ( ::compare(link->key,name) < 0 )
					continue;
				else
				if ( ::compare(link->key,name) == 0 )
					goto endFind;
				else	break;
			link = 0;
			}
		else
		while ( link = nextLink() )
			if ( ::compare(link->key,name) == 0 )
				goto endFind;
		}
endFind:
	entry = saveEntry;
	return link;
}

/***************************************************************************
	If it exists, return the first value in the table with key == name
***************************************************************************/
void *HashList::get(char *name)
{
HashLink 	*saveEntry = entry;
HashLink 	*link = 0;
	if ( hasher )
		link = hasher->find(name);
	else
	if ( hasKeys && name )
		{
		entry = 0;
		if ( sortAscending(isSorted) )
			{
			while ( link = nextLink() )
				if ( ::compare(link->key,name) < 0 )
					continue;
				else
				if ( ::compare(link->key,name) == 0 )
					goto endFind;
				else	break;
			link = 0;
			}
		else
		if ( sortDescending(isSorted) )
			{
			while ( link = priorLink() )
				if ( ::compare(link->key,name) < 0 )
					continue;
				else
				if ( ::compare(link->key,name) == 0 )
					goto endFind;
				else	break;
			link = 0;
			}
		else
		while ( link = nextLink() )
			if ( ::compare(link->key,name) == 0 )
				goto endFind;
		}
endFind:
	entry = saveEntry;
	if ( link )
		return link->value;
	return 0;
}

/***************************************************************************
	Factory to return a new link
***************************************************************************/
HashLink *HashList::getLink(void *item)
{
HashLink 	*link = 0;
	link = (HashLink*)HashList::globalHashTape->getStrip();
	link->value = item;
	link->list = this;
	return link;
}

/***************************************************************************
	Factory to return a new link with key set
***************************************************************************/
HashLink *HashList::getLink(char *n, void *item)
{
HashLink 	*link = getLink(item);
	link->key = n;
	return link;
}

/*****************************************************************************
	Insert an item at the beginning of the list. Note because there is no key
    no hashing takes place.
*****************************************************************************/
HashLink *HashList::insert(void *item)
{
	if ( !first )
		{
		first = last = getLink(item);
		first->list = this;
		hashListLength++;
		}
	else	first = first->insert(item);
	return first;
}

/*****************************************************************************
	Insert a keyed item at the beginning of the list. This pays no attention
    to list order.
*****************************************************************************/
HashLink *HashList::insert(char *name, void *item)
{
	insert(item);
	first->key = name;
	hasKeys = 1;
	if ( hasher )
		hasher->addHash(first);
	return first;
}

/***************************************************************************
	Iterator that goes thru the list in FIFO order. Uses the default
    entry.
***************************************************************************/
void *HashList::next()
{
	if ( !entry )
		entry = first;
	else	entry = entry->next;
	if ( entry )
		return entry->value;
	return 0;
}

/***************************************************************************
    A safer version of the iterator not subject to external interference.
***************************************************************************/
void *HashList::next(HashLink *&link)
{
	if ( !link )
		link = first;
	else	link = link->next;
	if ( link )
		return link->value;
	return 0;
}

/***************************************************************************
    Iterator that goes thru the list in FIFO order and returns the next
    link in the chain. Iterates using the default entry.
***************************************************************************/
HashLink *HashList::nextLink()
{
	if ( !entry )
		entry = first;
	else	entry = entry->next;
	return entry;
}

/***************************************************************************
    Pop treats the list as a stack and pops off the last item.
***************************************************************************/
void *HashList::pop()
{
void 	*stuff = (void*)0;
	if ( last )
		{
		stuff = last->value;
		last->remove();
		}
	return stuff;
}

/***************************************************************************
	Iterator that goes thru the list in LIFO order.
***************************************************************************/
void *HashList::prior()
{
	if ( !entry )
		entry = last;
	else	entry = entry->prior;
	if ( entry )
		return entry->value;
	return 0;
}

/***************************************************************************
    A safer version of the iterator not subject to external interference.
***************************************************************************/
void *HashList::prior(HashLink *&link)
{
	if ( !link )
		link = last;
	else	link = link->prior;
	if ( link )
		return link->value;
	return 0;
}

/***************************************************************************
    Iterator that goes thru the list back to front and returns the next
    link in the chain. Iterates using the default entry.
***************************************************************************/
HashLink *HashList::priorLink()
{
	if ( !entry )
		entry = last;
	else	entry = entry->prior;
	return entry;
}

/***************************************************************************
	Adds an entry or overwrites if already exists. No duplicates
***************************************************************************/
HashLink *HashList::put(char *n, void *i)
{
HashLink 	*link = find(n);
	if ( link )
		link->value = i;
	else	return add(n,i);
	return link;
}

HashLink *HashList::put(char *n)
{
	return put(n,(void*)n);
}

/*****************************************************************************
	Remove an item from the list. This is a brute force implementation that
    loops thru the list until it finds the item then removes it. The HashLink
    remove method cleans up the list as needed. Returns false if the removal
    empties the list.
*****************************************************************************/
int HashList::remove(void *item)
{
	for ( entry = first; entry; entry = entry->next )
		if ( entry->value == item )
			{
			entry->remove();
			entry = 0;
			break;
			}
	return hashListLength;
}

/***************************************************************************
    Just nulls out the entry.
***************************************************************************/
void HashList::resetIterator()
{
	entry = 0;
}

/***************************************************************************
    Sorts list using the compare method. The basic idea is to walk the list
    forward, and keep the already traversed part of the list in order.
    At the end of the list we done. If order is not null, sort is descending
***************************************************************************/
void HashList::sort(int (*comparisor)(HashLink *, HashLink *))
{
	if ( isSorted )
		{
		int 	order = sortDescending(isSorted) ? -1 : 1;
		entry = 0;
		while ( nextLink() )
			{
moveForward:
			HashLink *followLink = entry->next;
			if ( followLink )
				if ( order * comparisor(entry,followLink) <= 0 )
					continue;
				else {
					HashLink 	*link = entry;
					followLink->extract();
					while ( prior(link) )
						{
						if ( order * comparisor(followLink,link) < 0 )
							continue;
						else {
							link->append(followLink);
							goto moveForward;
							}
						}
					if ( !link && first )
						{
						first->insert(followLink);
						goto moveForward;
						}
					}
			}
		return;
		}
	::fprintf(stderr,"HashList sort: must set isSorted first\n");
}

/*****************************************************************************
	Returns a double indirection array of contents of this list
*****************************************************************************/
void **HashList::toArray()
{
void 	**atList = 0;
void 	**list = 0;
void 	*atData = 0;
	if ( !hashListLength )
		return 0;
	atList = list = (void**)::malloc((hashListLength + 1) * sizeof(void*));
	while ( atData = next() )
		{
		*atList++ = atData;
		}
	*atList = (void*)0;
	return list;
}
