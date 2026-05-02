#include <string.h>
#include <stdio.h>
#include "Tape.h"
#include "DoubleLinkList.h"
#include "DoubleLink.h"

DoubleLink::DoubleLink(void* item)
{
	key = 0;
	linkHash = 0;
	linkOrder = 0;
	list = 0;
	next = 0;
	prior = 0;
	value = (void*)item;
}

/*****************************************************************************
	Append a new link to this one
*****************************************************************************/
void DoubleLink::append(DoubleLink *link)
{
DoubleLink 	*save = next;
	next = link;
	next->prior = this;
	next->next = save;
	next->list = list;
	if ( save )
		save->prior = link;
	if ( list )
		{
		link->linkOrder = ++list->length;
		if ( !save )
			list->last = link;
		}
}

/*****************************************************************************
    Create a new link and append it to this one
*****************************************************************************/
DoubleLink *DoubleLink::append(void* item)
{
DoubleLink 	*link = list ? list->getLink((void*)item) : new DoubleLink(item);
	append(link);
	return link;
}

/*****************************************************************************
    Extract this link from the chain it is in. It leaves list current entry
    pointing to the entry that preceeds this extracted link so that you can
    next thru the list extracting or removing as you go.
*****************************************************************************/
void DoubleLink::extract()
{
	if ( !list )
		return;
	if ( list->entry = prior )
		prior->next = next;
	else	list->first = next;
	if ( next )
		next->prior = prior;
	else	list->last = prior;
	list->length--;
	if ( !list->length )
		list->first = list->entry = list->last = 0;
	list = 0;
}

/*****************************************************************************
	Insert the link passed in before this one.
*****************************************************************************/
void DoubleLink::insert(DoubleLink *toBeInserted)
{
DoubleLink 	*save = prior;
	toBeInserted->prior = save;
	toBeInserted->next = this;
	prior = toBeInserted;
	if ( save )
		save->next = toBeInserted;
	if ( list )
		{
		toBeInserted->list = list;
		toBeInserted->linkOrder = ++list->length;
		if ( !save )
			list->first = toBeInserted;
		}
}

/*****************************************************************************
    Create a new keyed link and insert it before this one. This does not change
    the current item on the parent list of this item.
*****************************************************************************/
DoubleLink *DoubleLink::insert(char *name, void* item)
{
DoubleLink 	*newEntry = list ? list->getLink((void*)item) : new DoubleLink(item);
	newEntry->key = name;
	insert(newEntry);
	return newEntry;
}

/*****************************************************************************
    Create a new link and insert it before this one. This does not change
    the current item on the parent list of this item.
*****************************************************************************/
DoubleLink *DoubleLink::insert(void* item)
{
DoubleLink 	*newEntry = list ? list->getLink((void*)item) : new DoubleLink(item);
	insert(newEntry);
	return newEntry;
}

/*****************************************************************************
	Remove this link from the chain it is in and return to the reuse list
*****************************************************************************/
void DoubleLink::remove()
{
	extract();
	DoubleLinkList::globalLinkTape->free(this);
}
