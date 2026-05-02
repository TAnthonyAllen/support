/*****************************************************************************
	Buffer class provides a string buffer, and if a file is set, a print buffer
*****************************************************************************/

class Buffer
{
public:
char *bufferName;
int fd;
int roomLeft;
int size;
char *file;
char *current;
char *end;
char *mark;
char *start;
void appendChar(char c);
void appendCount(int i, char *format);
void appendCount(int i);
void appendNumber(double d, char *format);
void appendNumber(double d);
void appendString(char *text, char *format);
void appendString(char *text);
void backupToMark();
int closeFile();
void deleteFromBuffer(int count);
void extend(int len);
void flush();
char *getMarkedString();
void insertIntoBuffer(char *text, int offset);
int length();
void reSize(int length);
void reset();
void setFile(char *name);
void setMark();
char *string();
void tabRight(int count);
char *tail(int offset);
char *toString();
void unMark();
};
Buffer *bufferFactory1();
Buffer *bufferFactory2(char *name);
Buffer *bufferFactory3(char *name, int buffersize);
Buffer *bufferFactory4(int buffersize);
