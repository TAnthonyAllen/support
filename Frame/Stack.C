#include <stdlib.h>
#include "Stack.h"

Stack::Stack()
{
	//right now this does not do anything
        top = 0;
	bottom = 0;
        size = 0;
	return;
}

/*******************************************************************************
    Clear the stack
*******************************************************************************/
void
Stack::clear()
{
    if (!size)
	return;
StackItem	*current = top;
    while ( top )
	{
	current = top->prior;
	free(top);
	top = current;
	}
    size = 0;
    bottom = 0;
}

/*******************************************************************************
        getTop returns the top of the queue (bottom of the stack)
*******************************************************************************/
void*
Stack::getTop()
{
    if ( top )
        return	top->value;
    return (void*)0;
}

/*******************************************************************************
        pop the next item off the top of the stack
*******************************************************************************/
void*
Stack::pop()
{
StackItem	*current = top;

	if ( top )
		{
		void	*value = current->value;
		top	= top->prior;
		if ( top )
			top->next = 0;
	        else    bottom = 0;
		free(current);
                size--;
		return	value;
		}
	return (void*)0;
}

/*******************************************************************************
        pop the next item off the front of the queue (bottom of stack)
*******************************************************************************/
void*
Stack::popFront()
{
    StackItem	*current = bottom;
    
    if ( bottom )
	{
	void	*value = current->value;
	bottom	= top->next;
	if ( bottom )
	    bottom->prior = 0;
	free(current);
	size--;
	return	value;
	}
    return (void*)0;
}

/*******************************************************************************
        push value onto stack
*******************************************************************************/
void
Stack::push( void *value )
{
StackItem	*current = (StackItem*)malloc(sizeof(StackItem));

	current->value	= value;
	current->prior	= top;
	if ( top )
		top->next = current;
        else    bottom  = current;
	top		= current;
        size++;
}

/*******************************************************************************
        push a stack onto stack
*******************************************************************************/
void
Stack::push( Stack *s )
{
	push( (void*)s );
}

/*******************************************************************************
        push an integer onto stack
*******************************************************************************/
void
Stack::push( int i )
{
	push( (void*)i );
}
