#include <string.h>
#include <stdio.h>
#include "BaseEntry.h"

BaseEntry::BaseEntry()
{
	key = 0;
	value = (void*)0;
	next = 0;
	nextHash = 0;
	prior = 0;
	priorHash = 0;
}

BaseEntry::BaseEntry(char *k, void *item)
{
	next = 0;
	nextHash = 0;
	prior = 0;
	priorHash = 0;
	key = k;
	value = item;
}

/******************************************************************************
	Append the entry passed in to this one in the ordered list
******************************************************************************/
BaseEntry *BaseEntry::append(BaseEntry *entry)
{
BaseEntry 	*old = next;
	next = entry;
	entry->prior = this;
	entry->next = old;
	if ( old )
		old->prior = entry;
	return entry;
}

/******************************************************************************
	Remove this entry from the ordered list and the hash list
        THIS NEEDS TO BE FIXED - perhaps call BaseHash remove but still need
        to make sure that does everything it needs to - definitely more complex
        than what is here
******************************************************************************/
void BaseEntry::remove()
{
	if ( priorHash )
		priorHash->nextHash = nextHash;
	if ( nextHash )
		nextHash->priorHash = priorHash;
	if ( prior )
		prior->next = next;
	if ( next )
		next->prior = prior;
}
