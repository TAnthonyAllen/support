class Buffer;
/*****************************************************************************
	PLGset class definition
*****************************************************************************/

class PLGset
{
public:
unsigned long *map;
char *name;
char *specs;
Buffer *setBuffer;
struct 
	{
	unsigned int deferUpdate:1;
	unsigned int hidden:1;
	unsigned int negated:1;
	unsigned int parsed:1;
	unsigned int setUpdated:1;
	};
static unsigned long *mapper;
PLGset();
PLGset(char *input);
void clear();
int contains(char c);
int contains(char *s);
int foundIn(char *text);
void generate(Buffer *output);
int isEmpty();
int length();
static void printText(char *text, Buffer *output);
void reset(int a);
void reset(PLGset *set);
void reset(int left, int right);
void set(int a);
void set(char *text);
void set(PLGset *set);
void set(int left, int right);
void setSimple(char *text);
char *skip(char *plgStart);
char *toString();
void updateBuffer();
};
