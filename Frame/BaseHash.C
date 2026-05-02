#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "StringRoutines.h"
#include "DoubleLinkList.h"
#include "DoubleLink.h"
#include "BaseHash.h"

/******************************************************************************
	Constructors
******************************************************************************/
BaseHash::BaseHash()
{
	hashSize = 255;
	hashStack = (DoubleLink**)::calloc(hashSize,sizeof(DoubleLink*));
	hashList = new DoubleLinkList();
}

BaseHash::BaseHash(int tableSize)
{
	hashSize = tableSize;
	hashStack = (DoubleLink**)::calloc(hashSize,sizeof(DoubleLink*));
	hashList = new DoubleLinkList();
}

/******************************************************************************
	Add entry, duplicates allowed. Hash list is FIFO except that duplicates are
    kept together.
******************************************************************************/
DoubleLink *BaseHash::add(char *name, void *entry)
{
	if ( name && entry )
		{
		int 		hashed = getHashIndex(name);
		int 		order = hashList->length;
		DoubleLink 	*atHash = hashStack[hashed];
		DoubleLink 	*newEntry = hashList->getLink(name,entry);
		newEntry->linkHash = hashed;
		if ( atHash )
			{
			DoubleLink 	*lastEntry = atHash;
			// the hash may match multiple names so we have to check
			while (atHash && atHash->linkHash == hashed && ::compare(atHash->key,name) < 0)
				{
				lastEntry = atHash;
				atHash = atHash->next;
				}
			// duplicates allowed so we scan for the last match entered
			if ( atHash && atHash->linkHash == hashed )
				{
				if ( ::compare(atHash->key,name) == 0 )
					while (atHash->next && ::compare(name,atHash->next->key) == 0)
						atHash = atHash->next;
				atHash->append(newEntry);
				}
			else	lastEntry->append(newEntry);
			}
		else {
			hashList->add(newEntry);
			hashStack[hashed] = newEntry;
			}
		newEntry->linkOrder = order++;
		return newEntry;
		}
	::fprintf(stderr,"BashHash add: got null argument passed in");
	return 0;
}

DoubleLink *BaseHash::add(char *name)
{
	return add(name,(void*)name);
}

/***************************************************************************
    clear all entries from the table and replace the hashStack
***************************************************************************/
void BaseHash::clear()
{
	hashList->clear();
	::free(hashStack);
	hashStack = (DoubleLink**)::calloc(hashSize,sizeof(DoubleLink*));
}

/***************************************************************************
	If it exists, return the first link with key == name
***************************************************************************/
DoubleLink *BaseHash::find(char *name)
{
	if ( name )
		{
		int 		hashed = getHashIndex(name);
		DoubleLink 	*atHash = hashStack[hashed];
		while (atHash && atHash->linkHash == hashed && ::compare(atHash->key,name) != 0)
			atHash = atHash->next;
		if ( atHash && ::compare(atHash->key,name) == 0 )
			return atHash;
		}
	return 0;
}

/***************************************************************************
	If it exists, return the first value in the table with key == name
***************************************************************************/
void *BaseHash::get(char *name)
{
	if ( name )
		{
		DoubleLink 	*atHash = find(name);
		if ( atHash )
			return atHash->value;
		}
	return 0;
}

/***************************************************************************
	Hash the name passed in and return the hash index
***************************************************************************/
int BaseHash::getHashIndex(char *name)
{
unsigned int 	h = 0;
char 			*p = name;
	while ( *p )
		{
		h <<= 5;
		h += *p++;
		}
	return h % hashSize;
}

/***************************************************************************
    List the keys for each entry.
***************************************************************************/
void BaseHash::listKeys()
{
	while ( hashList->entry = hashList->nextLink() )
		::printf("%s\n",hashList->entry->key);
}

/***************************************************************************
    Iterator that returns the next available entry w/key matching name.
    Note if the entry field is changed outside this method all bets are off.
***************************************************************************/
void *BaseHash::next(char *name)
{
	if ( name )
		if ( !hashList->entry )
			{
			hashList->entry = find(name);
			if ( hashList->entry )
				return hashList->entry->value;
			}
		else
		while ( hashList->entry = hashList->nextLink() )
			if ( ::compare(hashList->entry->key,name) == 0 )
				{
				return hashList->entry->value;
				}
	return 0;
}

/******************************************************************************
    Add an entry to the hash table and returns a pointer to the stored
    entry. Duplicates overwrite (only the last value is kept for a given key).
    List is kept in FIFO order.
******************************************************************************/
DoubleLink *BaseHash::put(char *name, void *entry)
{
	if ( name && entry )
		{
		int 		hashed = getHashIndex(name);
		DoubleLink 	*atHash = hashStack[hashed];
		DoubleLink 	*newEntry = hashList->getLink(name,entry);
		int 		order = hashList->length;
		newEntry->linkHash = hashed;
		if ( atHash )
			{
			DoubleLink 	*lastEntry = atHash;
			// the hash may match multiple names so we have to check
			while (atHash && atHash->linkHash == hashed && ::compare(atHash->key,name) < 0)
				{
				lastEntry = atHash;
				atHash = atHash->next;
				}
			// duplicates not allowed so we scan for the first match entered
			if ( atHash && atHash->linkHash == hashed )
				if ( ::compare(atHash->key,name) == 0 )
					atHash->value = entry;
				else	atHash->append(newEntry);
			else	lastEntry->append(newEntry);
			}
		else {
			hashList->add(newEntry);
			hashStack[hashed] = newEntry;
			newEntry->linkOrder = order++;
			}
		return newEntry;
		}
	::fprintf(stderr,"BashHash put: got null argument passed in");
	return 0;
}

DoubleLink *BaseHash::put(char *name)
{
	return put(name,(void*)name);
}

/*****************************************************************************
    Removes the first matching item found and returns it.
*****************************************************************************/
void *BaseHash::remove(char *name)
{
DoubleLink 	*atHash = find(name);
	if ( atHash )
		{
		void 	*data = atHash->value;
		removeFromHash(atHash);
		atHash->remove();
		return data;
		}
	return 0;
}

/*****************************************************************************
    Cleans up the hash when a link is removed.
*****************************************************************************/
void BaseHash::removeFromHash(DoubleLink *atHash)
{
DoubleLink 	*hashed = hashStack[atHash->linkHash];
	// check if we need to clean up the hashStack
	if ( atHash && atHash == hashed )
		{
		if ( atHash->next && atHash->next->linkHash == hashed->linkHash )
			hashStack[hashed->linkHash] = atHash->next;
		else	hashStack[hashed->linkHash] = 0;
		}
}
