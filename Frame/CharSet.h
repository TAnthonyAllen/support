class Buffer;
/*****************************************************************************
    CharSet — character-set utility class.

    Sister to PLGset, which adds parser-rule extras. CharSet is the clean
    character-set role: hold a set of chars, support add/remove/test/iterate.

    Representation: inSet[256] holds one byte per ASCII codepoint, 1 = in set,
    0 = not in set. Trivially debuggable — every byte is directly inspectable.
    Replaces the prior bit-packed map[4] of unsigned longs.

    Negation is resolved at set-construction time. A set built from "^abc"
    results in inSet[] with 253 entries set (all chars except a, b, c). The
    `negated` boolean is retained as an informational flag — set when ^ is
    parsed — but no method reads it for matching logic.

    setBuffer preserves the original source-form text (so "^abc" remains
    "^abc" for generation purposes). inSet[] holds the resolved set for
    matching.
*****************************************************************************/

class CharSet
{
public:
unsigned char *inSet;
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
CharSet();
CharSet(char *input);
void clear();
int contains(char c);
int contains(char *s);
int foundIn(char *text);
void generate(Buffer *output);
int isEmpty();
int length();
static void printText(char *text, Buffer *output);
void reset(int a);
void reset(CharSet *set);
void reset(int left, int right);
void set(int a);
void set(char *text);
void set(CharSet *set);
void set(int left, int right);
void setSimple(char *text);
char *skip(char *plgStart);
char *toString();
void updateBuffer();
};
