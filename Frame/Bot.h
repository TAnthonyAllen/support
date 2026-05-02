@class NSConnection;
@class NSObject;
/*****************************************************************************
	Bot class implements a remote object using NSConnection.
*****************************************************************************/

class Bot
{
public:
NSConnection *contact;
Bot(NSObject *object);
int registerAs(char *name);
};
