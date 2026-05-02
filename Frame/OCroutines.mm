#include <Cocoa/Cocoa.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Buffer.h"
#include "OCroutines.h"

/*******************************************************************************
	Method that fires a synchronous request. Returns true if it works, with the
    results added to the buffer passed in.
*******************************************************************************/
int getURLintoBuffer(char *url, Buffer *buffer)
{
NSURLRequest 	*request = ::getURLrequest(url);
NSURLResponse 	*response = 0;
NSError 		*error = 0;
NSData 			*data = 0;
	if ( request )
		if ( data = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error] )
			{
			buffer->appendString((char*)[data bytes]);
			return 1;
			}
	::fprintf(stderr,"%s\n",::toString([error localizedDescription]));
	return 0;
}

/*******************************************************************************
	Converts a string into a URLrequest
*******************************************************************************/
NSURLRequest *getURLrequest(char *url)
{
NSURL 			*wrap = 0;
NSURLRequest 	*request = 0;
	if ( url )
		{
		wrap = [[NSURL alloc] initWithString:[NSString stringWithCString:url encoding:NSASCIIStringEncoding]];
		if ( wrap )
			request = [[NSURLRequest alloc] initWithURL:wrap];
		else	::fprintf(stderr,"Could not create url from: %s\n",url);
		if ( !request )
			::fprintf(stderr,"Could not create request for: %s\n",url);
		}
	else	::fprintf(stderr,"No url provided\n");
	return request;
}

/*******************************************************************************
	Convert an Objective C NSString to a regular String
*******************************************************************************/
char *toString(NSString *s)
{
int 	length = (int)[s length] + 1;
char 	*text = (char*)::malloc(length);
	[s getCString:text maxLength:length encoding:NSASCIIStringEncoding];
	return text;
}
