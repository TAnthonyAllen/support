#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "StringRoutines.h"
#include "HashLink.h"
#include "Hasher.h"

/******************************************************************************
	Constructors
******************************************************************************/
Hasher::Hasher()
{
	hashSize = 255;
	hashStack = (HashLink**)::calloc(hashSize,sizeof(HashLink*));
}

Hasher::Hasher(int tableSize)
{
	hashSize = tableSize;
	hashStack = (HashLink**)::calloc(hashSize,sizeof(HashLink*));
}

/******************************************************************************
	Add entry, duplicates allowed.
******************************************************************************/
void Hasher::addHash(HashLink *newLink)
{
	if ( newLink )
		{
		char 	*name = newLink->key;
		newLink->linkHash = getHashIndex(name);
		HashLink *atHash = hashStack[newLink->linkHash];
		if ( atHash )
			{
			HashLink 	*lastEntry = atHash;
			// the hash may match multiple names so we have to check
			while (atHash && ::compare(atHash->key,name) < 0)
				{
				lastEntry = atHash;
				atHash = atHash->nextHash;
				}
			// duplicates allowed so we scan for the last match entered
			if ( atHash )
				{
				while ( atHash->nextHash && ::compare(name,atHash->nextHash->key) == 0 )
					atHash = atHash->nextHash;
				atHash->appendToHash(newLink);
				}
			else	lastEntry->appendToHash(newLink);
			}
		else	hashStack[newLink->linkHash] = newLink;
		}
	else	::fprintf(stderr,"Hasher add: got null argument passed in");
}

/***************************************************************************
    clear all entries from the table
***************************************************************************/
void Hasher::clear()
{
HashLink 	*last = 0;
HashLink 	*link = 0;
int 		i = 0;
	while ( i < hashSize )
		{
		last = hashStack[i];
		hashStack[i] = 0;
		while ( last )
			{
			link = last->nextHash;
			delete last;
			last = link;
			}
		i++;
		}
}

/***************************************************************************
	If it exists, return the first link with key == name
***************************************************************************/
HashLink *Hasher::find(char *name)
{
	if ( name )
		{
		int 		hashed = getHashIndex(name);
		HashLink 	*atHash = hashStack[hashed];
		while (atHash && atHash->linkHash == hashed && ::compare(atHash->key,name) != 0)
			atHash = atHash->nextHash;
		if ( atHash && ::compare(atHash->key,name) == 0 )
			return atHash;
		}
	return 0;
}

/***************************************************************************
	Hash the name passed in and return the hash index
***************************************************************************/
int Hasher::getHashIndex(char *name)
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

/*****************************************************************************
	Remove an item from the hash list.
*****************************************************************************/
void Hasher::remove(HashLink *link)
{
HashLink 	*last = 0;
HashLink 	*atHash = hashStack[link->linkHash];
	do	if ( link == atHash )
			{
			if ( last )
				{
				last->nextHash = atHash->nextHash;
				delete atHash;
				break;
				}
			last = atHash;
			}
	while ( atHash = atHash->nextHash );
}
