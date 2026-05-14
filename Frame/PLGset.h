class Buffer;
class PLGitem;
class CharSet;
/*****************************************************************************
    PLGset — parser-rule "set of tests" + character set.

    Sister class CharSet (support/Frame) handles the pure character-set role.
    PLGset is a superset: it has every character-set method CharSet has, plus
    the parser-rule extras (addTest, foundIn(PLGitem), generateNamed, setName,
    skip(PLGparse) overload) needed for PLG guard-set construction.

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

class PLGset
{
public:
char *name;
char *specs;
Buffer *setBuffer;
unsigned long *map;
struct 
	{
	unsigned int changed:1;
	unsigned int deferUpdate:1;
	unsigned int hidden:1;
	unsigned int negated:1;
	unsigned int parsed:1;
	};
static unsigned long *mapper;
unsigned char *inSet;
PLGset();
PLGset(char *input);
void clear();
int contains(char c);
int contains(char *s);
int foundIn(char *text);
int foundIn(PLGitem *item);
void generate(Buffer *output);
void generateNamed(Buffer *output);
void init();
int isEmpty();
int length();
static void printText(char *text, Buffer *output);
void reset(int a);
void reset(PLGset *set);
void reset(int left, int right);
void set(int a);
void set(char *text);
void set(PLGset *set);
void set(CharSet *other);
void set(int left, int right);
void setName();
void setSimple(char *text);
char *skip(char *plgStart);
char *toString();
char *toText();
void updateBuffer();
};
