#include <string.h>
#include <stdio.h>
#include "SearchItem.h"

SearchItem::SearchItem(void *entry)
{
	value = entry;
	next = 0;
}
