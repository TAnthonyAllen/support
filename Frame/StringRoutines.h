class PLGset;
class Buffer;
class StringRoutines
{
public:
static int debugIndent;
};
extern "C" char *alignLeft(char *text, int length);
extern "C" char *alignRight(char *text, int length);
extern "C" int bitCount(unsigned long bits);
extern "C" int booleanIsSet(unsigned char *item, int wordOffset, int bitOffset);
extern "C" int buttonIsSet(unsigned char *item, int wordOffset, int bitOffset, int length, int amount);
extern "C" int checkSys(int returned, char *message);
extern "C" int compare(char *left, char *right);
extern "C" int compareToStream(char *left, char *right);
extern "C" char *concat(int count, ...);
extern "C" int containsCharacter(char *text, char c);
extern "C" int containsString(char *text, char *substring);
extern "C" void convertNewLine(char *text);
extern "C" char *copyString(char *input);
extern "C" char *getStringFromFile(char *inputFile);
extern "C" char *headToChar(char *input, char match);
extern "C" char *headToCount(char *text, int count);
extern "C" char *headToSet(char *input, PLGset *matchSet);
extern "C" char *headToString(char *input, char *match);
extern "C" void indent(int indentify, char *indenter, Buffer *indentBuffer);
extern "C" void setBoolean(unsigned char *item, int wordOffset, int bitOffset, int value);
extern "C" void setButton(unsigned char *item, int wordOffset, int bitOffset, int length, int amount);
extern "C" int stringIsNumeric(char *text);
extern "C" char *tail(char *input, int length);
extern "C" char *toStringFromChar(char c);
extern "C" char *toStringFromDouble(double d);
extern "C" char *toStringFromFloat(float f);
extern "C" char *toStringFromInt(int i);
extern "C" char *toStringFromLong(long l);
extern "C" char *truncateString(char *text, int length);
extern "C" char *urlDecode(char *in);
