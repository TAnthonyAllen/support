#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "StringRoutines.h"
#include "Tape.h"
#include "DoubleLink.h"
#include "DoubleLinkList.h"
Tape *DoubleLinkList::globalLinkTape;

/*****************************************************************************
	Constructors uses default constructor
*****************************************************************************/
DoubleLinkList::DoubleLinkList()
{
	entry = 0;
	first = 0;
	last = 0;
	length = 0;
	hasKeys = 0;
	isSorted = 0;
	noDuplicates = 0;
	if ( !DoubleLinkList::globalLinkTape )
		DoubleLinkList::globalLinkTape = new Tape("globalLinkTape",50000,sizeof(DoubleLink));
}

/*****************************************************************************
	Add to the list
*****************************************************************************/
DoubleLink *DoubleLinkList::add(void* item)
{
	//if hasKeys cerr "DoubleLinkList add: no key supplied to hasKey list":;
	if ( !first )
		{
		first = last = getLink(item);
		last->list = this;
		length++;
		}
	else	last = last->append((void*)item);
	last->linkOrder = length;
	return last;
}

/*****************************************************************************
	If hasKeys uses name as a key when adding
*****************************************************************************/
DoubleLink *DoubleLinkList::add(char *name)
{
	if ( name )
		{
		if ( hasKeys )
			return add(name,(void*)name);
		return add((void*)name);
		}
	return 0;
}

/*****************************************************************************
	Add to the keyed list. If list is not sorted it just appends. In any
    case the link key is set.
*****************************************************************************/
DoubleLink *DoubleLinkList::add(char *n, void* i)
{
DoubleLink 	*link = 0;
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
		add(i);
		link = last;
		goto finishAdd;
		}
appendLink:
	link = link->append((void*)i);
	goto finishAdd;
insertLink:
	link = link->insert((void*)i);
finishAdd:
	link->key = n;
	return link;
}

DoubleLink *DoubleLinkList::add(DoubleLink *link)
{
	//if hasKeys cerr "DoubleLinkList add link: no key supplied to hasKey list":;
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
	link->linkOrder = ++length;
	return link;
}

/*****************************************************************************
	Add to a keyed list in link order (default is in ascending order if the
    list is not sorted).
*****************************************************************************/
void DoubleLinkList::addInLinkOrder(DoubleLink *inputLink)
{
DoubleLink 	*link = 0;
int 		flip = sortDescending(isSorted) ? -1 : 1;
	entry = 0;
	while ( link = nextLink() )
		{
		int 	ordered = flip * (inputLink->linkOrder - link->linkOrder);
		if ( ordered > 0 )
			if ( link->next )
				continue;
			else	goto appendLink;
		else
		if ( ordered == 0 )
			goto appendLink;
		else	goto insertLink;
		}
	if ( !link )
		{
		add((void*)inputLink->value);
		link = last;
		goto finishAdd;
		}
appendLink:
	link = link->append(inputLink->value);
	goto finishAdd;
insertLink:
	link = link->insert(inputLink->value);
finishAdd:
	link->key = inputLink->key;
	link->linkOrder = inputLink->linkOrder;
}

/*****************************************************************************
	Clear the list and release its resources (for reuse if globalLinkTape is set)
*****************************************************************************/
void DoubleLinkList::clear()
{
	for ( entry = first; entry; entry = entry->next )
		if ( DoubleLinkList::globalLinkTape )
			DoubleLinkList::globalLinkTape->free((void*)entry);
		else
		delete entry;
	first = entry = last = 0;
	length = 0;
}

/***************************************************************************
	Like pop treats the list as stack but pops off first item, not last.
***************************************************************************/
void *DoubleLinkList::dQ()
{
void* 	stuff = (void*)0;
	if ( first )
		{
		stuff = (void*)first->value;
		first->remove();
		}
	return stuff;
}

/***************************************************************************
	If it exists, return the first link with key == name. The search takes
    advantage of sort status. If not sorted, examines sequentially. It
    leaves current entry as is.
***************************************************************************/
DoubleLink *DoubleLinkList::find(char *name)
{
DoubleLink 	*saveEntry = entry;
DoubleLink 	*link = 0;
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
void *DoubleLinkList::get(char *name)
{
DoubleLink 	*link = find(name);
	if ( link )
		return link->value;
	return 0;
}

/***************************************************************************
	Factory to return a new link
***************************************************************************/
DoubleLink *DoubleLinkList::getLink(void* item)
{
DoubleLink 	*link = 0;
	link = (DoubleLink*)DoubleLinkList::globalLinkTape->getStrip();
	::memset(link,0,sizeof(DoubleLink));
	link->value = (void*)item;
	link->list = this;
	return link;
}

/***************************************************************************
	Factory to return a new link with key set
***************************************************************************/
DoubleLink *DoubleLinkList::getLink(char *n, void* item)
{
DoubleLink 	*link = getLink(item);
	link->key = n;
	return link;
}

/*****************************************************************************
	Insert an item at the beginning of the list.
*****************************************************************************/
DoubleLink *DoubleLinkList::insert(void* item)
{
	if ( !first )
		{
		first = last = getLink(item);
		first->list = this;
		length++;
		}
	else	first = first->insert((void*)item);
	return first;
}

/*****************************************************************************
	Insert a keyed item at the beginning of the list. This pays no attention
    to list order.
*****************************************************************************/
DoubleLink *DoubleLinkList::insert(char *name, void* item)
{
	if ( !first )
		{
		first = last = getLink(item);
		first->list = this;
		length++;
		}
	else	first = first->insert((void*)item);
	first->key = name;
	hasKeys = 1;
	return first;
}

/***************************************************************************
	Iterator that goes thru the list in FIFO order. Uses the default
    entry.
***************************************************************************/
void *DoubleLinkList::next()
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
void *DoubleLinkList::next(DoubleLink *&link)
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
DoubleLink *DoubleLinkList::nextLink()
{
	if ( !entry )
		entry = first;
	else	entry = entry->next;
	return entry;
}

/***************************************************************************
    Pop treats the list as a stack and pops off the last item.
***************************************************************************/
void *DoubleLinkList::pop()
{
void* 	stuff = (void*)0;
	if ( last )
		{
		stuff = (void*)last->value;
		last->remove();
		}
	return stuff;
}

/***************************************************************************
	Iterator that goes thru the list in LIFO order.
***************************************************************************/
void *DoubleLinkList::prior()
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
void *DoubleLinkList::prior(DoubleLink *&link)
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
DoubleLink *DoubleLinkList::priorLink()
{
	if ( !entry )
		entry = last;
	else	entry = entry->prior;
	return entry;
}

/***************************************************************************
	Adds an entry or overwrites if already exists. No duplicates
***************************************************************************/
DoubleLink *DoubleLinkList::put(char *n, void* i)
{
DoubleLink 	*link = find(n);
	if ( link )
		link->value = (void*)i;
	else	return add(n,i);
	return link;
}

DoubleLink *DoubleLinkList::put(char *n)
{
	return put(n,(void*)n);
}

/*****************************************************************************
	Remove an item from the list. This is a brute force implementation that
    loops thru the list until it finds the item then removes it. The DoubleLink
    remove method cleans up the list as needed. Returns false if the removal
    empties the list.
*****************************************************************************/
int DoubleLinkList::remove(void* item)
{
	for ( entry = first; entry; entry = entry->next )
		if ( entry->value == item )
			{
			entry->remove();
			entry = 0;
			break;
			}
	return length;
}

/***************************************************************************
    Just nulls out the entry.
***************************************************************************/
void DoubleLinkList::resetIterator()
{
	entry = 0;
}

/***************************************************************************
    Sorts list using the compare method. The basic idea is to walk the list
    forward, and keep the already traversed part of the list in order.
    At the end of the list we done. If order is not null, sort is descending
***************************************************************************/
void DoubleLinkList::sort(int (*comparisor)(DoubleLink *, DoubleLink *))
{
	if ( isSorted )
		{
		int 	order = sortDescending(isSorted) ? -1 : 1;
		entry = 0;
		while ( nextLink() )
			{
moveForward:
			DoubleLink *followLink = entry->next;
			if ( followLink )
				if ( order * comparisor(entry,followLink) <= 0 )
					continue;
				else {
					DoubleLink 	*link = entry;
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
	::fprintf(stderr,"DoubleLinkList sort: must set isSorted first\n");
}

/*****************************************************************************
	Returns a double indirection array of contents of this list
*****************************************************************************/
void **DoubleLinkList::toArray()
{
void 	**atList = 0;
void 	**list = 0;
void 	*atData = 0;
	if ( !length )
		return 0;
	atList = list = (void**)::malloc((length + 1) * sizeof(void*));
	while ( atData = next() )
		{
		*atList++ = atData;
		}
	*atList = (void*)0;
	return list;
}
