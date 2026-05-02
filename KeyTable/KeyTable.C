#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "KeyMap.h"
#include "DoubleLinkList.h"
#include "DoubleLink.h"
#include "PLGset.h"
#include "KeyTableItem.h"
#include "KeyTable.h"

KeyTable::KeyTable()
{
	debug = 0;
	max = 0;
	size = 0;
	current = 0;
	first = 0;
	last = 0;
	keyValueList = 0;
	name = 0;
	letterMaps = new DoubleLinkList();
	keySet = new PLGset();
}

KeyTable::KeyTable(char *n)
{
	debug = 0;
	max = 0;
	size = 0;
	current = 0;
	first = 0;
	last = 0;
	keyValueList = 0;
	name = n;
	letterMaps = new DoubleLinkList();
	keySet = new PLGset();
	keySet->name = n;
}

/*******************************************************************************
	Add something to the table, indexed by key. If the key is already in the
	table, nothing happens.
*******************************************************************************/
int KeyTable::add(char *key)
{
	return add(key,(void*)key);
}

int KeyTable::add(char *key, void *pointer)
{
char 			*atLetter = 0;
KeyTableItem 	*item = new KeyTableItem(key,pointer);
DoubleLink 		*link = letterMaps->first;
KeyMap 			**atMap = 0;
KeyMap 			*map = 0;
int 			offset = 0;
int 			depth = 0;
	if ( keyValueList )
		{
		::free((void*)keyValueList);
		keyValueList = 0;
		}
	for ( atLetter = key, depth = 1; *atLetter; atLetter++, depth++ )
		{
		if ( atLetter > key )
			map->follow->set((int)*atLetter);
		if ( !link )
			link = newLink();
		atMap = (KeyMap**)link->value;
		offset = (int)*atLetter;
		atMap += offset;
		if ( !*atMap )
			{
			//cout "KeyTable: new map":;
			map = new KeyMap(*atLetter);
			map->length = depth;
			*atMap = map;
			}
		else	map = *atMap;
		//cout `"map letter:",map.letter:;
		link = link->next;
		if ( !*(atLetter + 1) )
			{
			if ( map->find(key) )
				return 0;
			map->entries->add((void*)item);
			item->position = ++size;
			if ( map->entries->length > max )
				max = map->entries->length;
			if ( last )
				last->next = item;
			last = item;
			if ( !first )
				first = item;
			break;
			}
		}
	keySet->set(*key);
	return 1;
}

/*******************************************************************************
	List out the contents of the table. If the flag is set, assumes that the
	table contents are strings and prints them out
*******************************************************************************/
void KeyTable::dump(int flag)
{
KeyTableItem 	*item = 0;
	item = getNext();
	while ( item )
		{
		if ( flag )
			printf("%s %s\n",item->key,(char*)item->value);
		else	printf("%s\n",item->key);
		item = getNext();
		}
}

/*******************************************************************************
	Return whatever is in the table indexed by key
*******************************************************************************/
void *KeyTable::find(char *key)
{
KeyTableItem 	*item = 0;
	if ( item = findKeyItem(key) )
		return item->value;
	return (void*)0;
}

/*******************************************************************************
	Return the key item indexed by key
*******************************************************************************/
KeyTableItem *KeyTable::findKeyItem(char *key)
{
char 			*atLetter = 0;
KeyTableItem 	*item = 0;
DoubleLink 		*link = letterMaps->first;
KeyMap 			**atMap = 0;
KeyMap 			*map = 0;
int 			offset = 0;
	for ( atLetter = key; *atLetter; atLetter++ )
		{
		if ( atLetter > key && !map->follow->contains((int)*atLetter) )
			break;
		if ( !link )
			break;
		atMap = (KeyMap**)link->value;
		offset = (int)*atLetter;
		atMap += offset;
		map = *atMap;
		if ( !map )
			break;
		link = link->next;
		if ( !*(atLetter + 1) )
			{
			item = map->find(key);
			break;
			}
		}
	if ( item )
		return item;
	return (KeyTableItem*)0;
}

/*******************************************************************************
    Iterator that returns the next item. Definitely not re-entrant
*******************************************************************************/
KeyTableItem *KeyTable::getNext()
{
	if ( !current )
		current = first;
	else	current = current->next;
	return current;
}

/*******************************************************************************
    Accesses the key table as if it is an array returning the value at the
    indexed element. Note that the index passed in starts at 1
*******************************************************************************/
void *KeyTable::itemAt(int index)
{
void 	*itemValue = 0;
void 	**atList = 0;
	if ( index > 0 && index <= size )
		{
		if ( !keyValueList )
			{
			current = 0;
			atList = keyValueList = (void**)::calloc(size + 1,sizeof(void*));
			while ( itemValue = next() )
				{
				*atList++ = itemValue;
				}
			}
		return *(keyValueList + --index);
		}
	return 0;
}

/*******************************************************************************
	Return the longest entry in the table that matches the beginning of key
*******************************************************************************/
KeyTableItem *KeyTable::matches(char *key)
{
char 			*atLetter = 0;
KeyTableItem 	*item = 0;
KeyTableItem 	*match = 0;
DoubleLink 		*link = letterMaps->first;
KeyMap 			**atMap = 0;
KeyMap 			*map = 0;
int 			offset = 0;
	for ( atLetter = key; *atLetter; atLetter++ )
		{
		if ( atLetter > key && !map->follow->contains((int)*atLetter) )
			break;
		if ( !link )
			break;
		atMap = (KeyMap**)link->value;
		offset = (int)*atLetter;
		atMap += offset;
		map = *atMap;
		if ( !map )
			break;
		link = link->next;
		if ( !*(atLetter + 1) || (map && map->entries->length) )
			{
			match = map->find(key);
			if ( match )
				item = match;
			}
		}
	if ( item )
		return item;
	return 0;
}

/*******************************************************************************
	Get a new DoubleLink item of pointers to KeyMaps
*******************************************************************************/
DoubleLink *KeyTable::newLink()
{
KeyMap 		**maps = (KeyMap**)::calloc(256,sizeof(KeyMap*));
DoubleLink 	*link = letterMaps->add((void*)maps);
	//cout ``"newLink":;
	return link;
}

/*******************************************************************************
    Iterator that returns the next item value. Definitely not re-entrant
*******************************************************************************/
void *KeyTable::next()
{
	if ( !current )
		current = first;
	else	current = current->next;
	if ( current )
		return current->value;
	return 0;
}
