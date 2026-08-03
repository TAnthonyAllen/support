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
struct 
	{
	unsigned int markIsSet:1;
	};
Buffer();
Buffer(char *name, int buffersize);
Buffer(char *name);
void appendChar(char c, char *format, int width);
void appendDouble(double d, char *format, int width);
void appendFloat(float f, char *format, int width);
void appendInt(int i, char *format, int width);
void appendLong(long l, char *format, int width);
void appendString(char *text, char *format, int width);
void backupToMark();
int closeFile();
int compress();
int decompress();
void deleteFromBuffer(int count);
void extend(int len);
int findInBuffer(char *needle);
void flush();
char *getMarkedString();
void insertAtMark(char *text, int length);
void insertIntoBuffer(char *text, int offset);
int length();
void reSize(int length);
void reset();
void setFile(char *name);
void setMark();
void shiftAtMark(int length);
void shorten(int count);
char *string();
void tabRight(int count);
char *tail(int offset);
char *toString();
void unMark();
};
