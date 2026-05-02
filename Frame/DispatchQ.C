#include <dispatch/dispatch.h>
#include <string.h>
#include <stdio.h>
#include "stdlib.h"
#include "DispatchQ.h"

/*****************************************************************************
	Construct a labelled queue
*****************************************************************************/
DispatchQ::DispatchQ(char *label)
{
	count = 0;
	interval = 0;
	data = (void*)0;
	qu = ::dispatch_queue_create(label,0);
}

DispatchQ::DispatchQ()
{
	count = 0;
	interval = 0;
	data = (void*)0;
	qu = ::dispatch_get_global_queue(0,0);
}

/*****************************************************************************
	Construct using a global queue (if no argument is passed, gets the
	default queue).
*****************************************************************************/
DispatchQ::DispatchQ(int priority)
{
	count = 0;
	interval = 0;
	data = (void*)0;
	qu = ::dispatch_get_global_queue(priority,0);
}

/*****************************************************************************
	Dispatch block on Q after interval nanoseconds or if dispatchGroup is set,
	does a dispatch notify to run block after dispatchGroup finishes
*****************************************************************************/
void DispatchQ::after(void (^block)())
{
unsigned long 	time = 0;
	if ( dispatchGroup )
		::dispatch_group_notify(dispatchGroup,qu,block);
	else {
		time = ::dispatch_time(DISPATCH_TIME_NOW,interval);
		::dispatch_after(time,qu,block);
		}
}

/*****************************************************************************
	Dispatch method on Q after delay nanoseconds or if dispatchGroup is set,
	does a dispatch notify to run block after dispatchGroup finishes
*****************************************************************************/
void DispatchQ::after(void (*method)(void *))
{
unsigned long 	time = 0;
	if ( dispatchGroup )
		::dispatch_group_notify_f(dispatchGroup,qu,data,method);
	else {
		time = ::dispatch_time(DISPATCH_TIME_NOW,interval);
		::dispatch_after_f(time,qu,data,method);
		}
}

/*****************************************************************************
	dispatch block on queue. This dispatches multiple instances of the block
    passed in, passing an interation number to each block that gets run
*****************************************************************************/
void DispatchQ::repeat(void (^block)(int ))
{
	if ( !count )
		::fprintf(stderr,"Repeat count not set\n");
	else	::dispatch_apply(count,qu,(void(^)(size_t))block);
}

/*****************************************************************************
	dispatch method on queue. This dispatches multiple instances of the method
    passed in, passing an interation number to each method that gets run
*****************************************************************************/
void DispatchQ::repeat(void (*method)(void *, int ))
{
	if ( !count )
		::fprintf(stderr,"Repeat count not set\n");
	else	::dispatch_apply_f(count,qu,data,(void(*)(void*,size_t))method);
}

/*****************************************************************************
	Submit block to queue to run concurrently. If dispatchGroup is set, a group
	dispatch is used
*****************************************************************************/
void DispatchQ::run(void (^block)())
{
	if ( dispatchGroup )
		::dispatch_group_async(dispatchGroup,qu,block);
	else	::dispatch_async(qu,block);
}

/*****************************************************************************
	Submit method to queue to run concurrently. If dispatchGroup is set, a group
	dispatch is used
*****************************************************************************/
void DispatchQ::run(void (*method)(void *))
{
	if ( dispatchGroup )
		::dispatch_group_async_f(dispatchGroup,qu,data,method);
	else	::dispatch_async_f(qu,data,method);
}

/*****************************************************************************
	Wait for dispatchGroup to finish processing
*****************************************************************************/
void DispatchQ::wait(dispatch_time_t time)
{
	::dispatch_group_wait(dispatchGroup,time);
}
