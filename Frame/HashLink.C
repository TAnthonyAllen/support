#include <string.h>
#include <stdio.h>
#include "Tape.h"
#include "DoubleLinkList.h"
#include "HashList.h"
#include "Hasher.h"
#include "HashLink.h"

HashLink::HashLink(void *item)
{
	key = 0;
	linkHash = 0;
	list = 0;
	next = 0;
	prior = 0;
	nextHash = 0;
	value = item;
}

/*****************************************************************************
	Append a new link to this one
*****************************************************************************/
void HashLink::append(HashLink *link)
{
HashLink 	*save = next;
	next = link;
	next->prior = this;
	next->next = save;
	next->list = list;
	if ( save )
		save->prior = link;
	if ( list && !save )
		list->last = link;
	list->hashListLength++;
}

/*****************************************************************************
    Create a new link and append it to this one
*****************************************************************************/
HashLink *HashLink::append(void *item)
{
HashLink 	*link = list ? list->getLink(item) : list->getLink(item);
	append(link);
	return link;
}

/*****************************************************************************
    Append the link passed in to this one
*****************************************************************************/
void HashLink::appendToHash(HashLink *link)
{
	link->nextHash = nextHash;
	nextHash = link;
	list->hashListLength++;
}

/*****************************************************************************
    Extract this link from the chain it is in. It leaves list current entry
    pointing to the entry that preceeds this extracted link so that you can
    next thru the list extracting or removing as you go.
*****************************************************************************/
void HashLink::extract()
{
	if ( !list )
		return;
	if ( list->entry = prior )
		prior->next = next;
	else	list->first = next;
	if ( next )
		next->prior = prior;
	else	list->last = prior;
	list->hashListLength--;
	if ( !list->hashListLength )
		list->first = list->entry = list->last = 0;
	list = 0;
}

/*****************************************************************************
	Insert the link passed in before this one.
*****************************************************************************/
void HashLink::insert(HashLink *toBeInserted)
{
HashLink 	*save = prior;
	toBeInserted->prior = save;
	toBeInserted->next = this;
	prior = toBeInserted;
	if ( save )
		save->next = toBeInserted;
	if ( list )
		{
		toBeInserted->list = list;
		if ( !save )
			list->first = toBeInserted;
		}
	list->hashListLength++;
}

/*****************************************************************************
    Create a new keyed link and insert it before this one. This does not change
    the current item on the parent list of this item.
*****************************************************************************/
HashLink *HashLink::insert(char *name, void *item)
{
HashLink 	*newEntry = list ? list->getLink(item) : list->getLink(item);
	newEntry->key = name;
	insert(newEntry);
	if ( list && list->hasher )
		list->hasher->addHash(newEntry);
	return newEntry;
}

/*****************************************************************************
    Create a new link and insert it before this one. This does not change
    the current item on the parent list of this item.
*****************************************************************************/
HashLink *HashLink::insert(void *item)
{
HashLink 	*newEntry = list ? list->getLink(item) : list->getLink(item);
	insert(newEntry);
	return newEntry;
}

/*****************************************************************************
	Remove this link from the chain it is in and return to the reuse list
*****************************************************************************/
void HashLink::remove()
{
	if ( list->hasher )
		list->hasher->remove(this);
	extract();
	DoubleLinkList::globalLinkTape->free(this);
}
