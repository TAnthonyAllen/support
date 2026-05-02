/*****************************************************************************
	Class is a wrapper for GCD dispatching
*****************************************************************************/

class DispatchQ
{
public:
int count;
int interval;
dispatch_queue_t qu;
void *data;
dispatch_group_t dispatchGroup;
DispatchQ(char *label);
DispatchQ();
DispatchQ(int priority);
void after(void (^block)());
void after(void (*method)(void *));
void repeat(void (^block)(int ));
void repeat(void (*method)(void *, int ));
void run(void (^block)());
void run(void (*method)(void *));
void wait(dispatch_time_t time);
};
