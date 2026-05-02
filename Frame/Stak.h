/*****************************************************************************
	A simple stack that will resize if it runs out of room. Default size is
	100 and will double each time it resizes
*****************************************************************************/

class Stak
{
public:
int length;
int size;
void **start;
void **end;
void **entry;
void **fifo;
Stak();
Stak(int s);
void clear();
void *next();
void *pop();
void popOff(int i);
void *prior();
void push(void *v);
void resize();
void *top();
};
