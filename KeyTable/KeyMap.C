#include <string.h>
#include <stdio.h>
#include "DoubleLinkList.h"
#include "DoubleLink.h"
#include "PLGset.h"
#include "KeyTableItem.h"
#include "KeyMap.h"

// All entries will have keys this long
KeyMap::KeyMap(char l)
{
	length = 0;
	letter = l;
	follow = new PLGset();
	entries = new DoubleLinkList();
}

/*******************************************************************************
	Return the value of the entry indexed by key if it exists
*******************************************************************************/
KeyTableItem *KeyMap::find(char *key)
{
DoubleLink 		*link = 0;
KeyTableItem 	*item = 0;
	for ( link = entries->first; link; link = link->next )
		{
		item = (KeyTableItem*)link->value;
		if ( !::strncmp(key,item->key,length) )
			return item;
		}
	return 0;
}
