/******************************************************************************
	The Stack class is a simple linked list stack that also works as a
	queue
******************************************************************************/
class StackItem
{
public:
	void		*value;
	StackItem	*next,
			*prior;
};

class Stack
{
public:
	StackItem	*bottom,
			*top;
        int		size;
			Stack();
	void		clear();
        void		*getTop();
	void		*pop();
	void		*popFront();
	void		push( int );
	void		push( Stack* );
	void		push( void* );
};
