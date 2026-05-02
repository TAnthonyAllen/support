class Buffer;
@class NSURLRequest;
@class NSString;
/*******************************************************************************
	Objective C global routines
*******************************************************************************/

class OCroutines
{
public:
};
int getURLintoBuffer(char *url, Buffer *buffer);
NSURLRequest *getURLrequest(char *url);
char *toString(NSString *s);
