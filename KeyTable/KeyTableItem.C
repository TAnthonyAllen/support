#include <string.h>
#include <stdio.h>
#include "KeyTableItem.h"

KeyTableItem::KeyTableItem(char *k, void *v)
{
	itemText = 0;
	position = 0;
	key = k;
	value = v;
	next = 0;
}
