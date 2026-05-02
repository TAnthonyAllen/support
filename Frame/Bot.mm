#include <Cocoa/Cocoa.h>
#include <string.h>
#include <stdio.h>
#include "OCroutines.h"
#include "Bot.h"

/*****************************************************************************
	Construct this Bot to handle the object passed in. Note the current run
    loop has to be running before the bot will do anything.
*****************************************************************************/
Bot::Bot(NSObject *object)
{
	contact = [[NSConnection alloc] init];
	[contact setRootObject:object];
}

/*****************************************************************************
	Register this Bot using the name passed in.
*****************************************************************************/
int Bot::registerAs(char *name)
{
	if ( ![contact registerName:[NSString stringWithCString:name encoding:NSASCIIStringEncoding]] )
		{
		::fprintf(stderr,"Bot registration failed for: %s\n",name);
		return 0;
		}
	::printf("Bot registration of %s successful\n",name);
	return 1;
}
