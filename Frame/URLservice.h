@class NSURLConnection;
class Buffer;
@class NSError;
@class NSURLResponse;
@class NSData;
/*******************************************************************************
	A wrapper for NSURLconnection
*******************************************************************************/
@interface URLservice : NSObject
{
@public
NSURLConnection *connect;
char *filename;
Buffer *buffer;
}
- (void)connection:(NSURLConnection*)c didFailWithError:(NSError*)didFailWithError;
- (void)connection:(NSURLConnection*)c didReceiveResponse:(NSURLResponse*)didReceiveResponse;
- (void)connection:(NSURLConnection*)c didReceiveData:(NSData*)didReceiveData;
- (void)connectionDidFinishLoading:(NSURLConnection*)c;
- (id)init:(Buffer*)b;
- (void)send:(char*)url;
@end
