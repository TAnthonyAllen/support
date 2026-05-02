#import <Cocoa/Cocoa.h>
#import <string.h>
#import <stdio.h>
#import "OCroutines.h"
#import "Buffer.h"
#import "URLservice.h"

@implementation URLservice

- (void)connection:(NSURLConnection*)c didFailWithError:(NSError*)didFailWithError
{
	::fprintf(stderr,"%s\n",::toString([didFailWithError localizedDescription]));
	::CFRelease((CFTypeRef)connect);
}

- (void)connection:(NSURLConnection*)c didReceiveResponse:(NSURLResponse*)didReceiveResponse
{
	::printf("Got a response from url\n");
}

- (void)connection:(NSURLConnection*)c didReceiveData:(NSData*)didReceiveData
{
	buffer->appendString((char*)[didReceiveData bytes],0,0);
}

- (void)connectionDidFinishLoading:(NSURLConnection*)c
{
	if ( buffer->file == filename )
		{
		buffer->closeFile();
		}
	::CFRelease((CFTypeRef)connect);
}

- (id)init:(Buffer*)b
{
	buffer = b;
	return self;
}

- (void)send:(char*)url
{
NSURLRequest 	*request = ::getURLrequest(url);
	if ( request )
		{
		connect = [[NSURLConnection alloc] initWithRequest:request delegate:self];
		::CFRetain((CFTypeRef)connect);
		}
}
@end
