/*****************************************************************************
    bufferPop -- the isolation POP for Buffer::compress / Buffer::decompress.

    THIS DRIVER MEASURES; bufferPop.sh ASSERTS.  Every quantity is printed
    unconditionally as a "VAL name = value" or "STR name = value" line, so no
    check can pass by a line going missing (RULE H4, presence-with-value).
    Diagnostics go to stderr; the VAL/STR lines are the product and go to
    stdout.  The last line printed is the sentinel and is reachable only
    through the final section (RULE H2).
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Buffer.h"

static void val(const char *name, long v)
{
    printf("VAL %s = %ld\n", name, v);
}

static void str(const char *name, const char *v)
{
    printf("STR %s = %s\n", name, v);
}

/*  Fill a Buffer with exactly n bytes, verbatim, NULs included.  Direct field
    writes are deliberate: no Buffer accessor can place a zero byte, and the
    byte-exactness claim is worthless if the fixture cannot express one.       */
static Buffer *loaded(const char *data, int n)
{
    Buffer *b = new Buffer((char *)"pop", n + 16);
    if (n) memcpy(b->start, data, (size_t)n);
    b->current  = b->start + n;
    *b->current = 0;
    b->roomLeft = (int)(b->end - b->current);
    return b;
}

static int alphabetClean(const char *s, int n)
{
    for (int i = 0; i < n; i++) {
        char c = s[i];
        int ok = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
              || (c >= '0' && c <= '9') || c == ':' || c == '-' || c == '_';
        if (!ok) {
            fprintf(stderr, "bufferPop: byte %d of compressed form is 0x%02x\n",
                    i, (unsigned char)c);
            return 0; }
    }
    return 1;
}

static void roundTrip(const char *tag, const char *data, int n)
{
    char name[128];
    Buffer *b = loaded(data, n);

    int origLen = b->length();
    int okc     = b->compress();
    int comLen  = b->length();

    char *com = (char *)malloc((size_t)comLen + 1);
    if (comLen) memcpy(com, b->start, (size_t)comLen);
    com[comLen] = 0;

    int okd     = b->decompress();
    int backLen = b->length();
    int same    = (backLen == n) && (n == 0 || memcmp(b->start, data, (size_t)n) == 0);

    sprintf(name, "%s.compressOK",    tag); val(name, okc);
    sprintf(name, "%s.decompressOK",  tag); val(name, okd);
    sprintf(name, "%s.rawLen",        tag); val(name, origLen);
    sprintf(name, "%s.backLen",       tag); val(name, backLen);
    sprintf(name, "%s.identical",     tag); val(name, same ? 1 : 0);
    sprintf(name, "%s.comLen",        tag); val(name, comLen);
    sprintf(name, "%s.alphabet",      tag); val(name, alphabetClean(com, comLen));
    sprintf(name, "%s.mode",          tag);
    { char m[2]; m[0] = comLen > 4 ? com[4] : '?'; m[1] = 0; str(name, m); }
    sprintf(name, "%s.pct",           tag);
    val(name, origLen ? (long)comLen * 100 / origLen : 0);

    free(com);
    delete b;
}

int main(void)
{
    /*  SECTION 1 -- the three cases the charter names, plus byte-exactness.   */

    roundTrip("empty", "", 0);

    roundTrip("hello", "hello", 5);

    {   char text[2048];
        const char *line = "the quick brown fox jumps over the lazy dog; ";
        int at = 0;
        while (at < (int)sizeof(text)) {
            int k = (int)strlen(line);
            if (at + k > (int)sizeof(text)) k = (int)sizeof(text) - at;
            memcpy(text + at, line, (size_t)k);
            at += k; }
        roundTrip("text", text, (int)sizeof(text)); }

    {   /*  every byte value 0..255, so the zero byte is exercised head on.    */
        char bytes[256];
        for (int i = 0; i < 256; i++) bytes[i] = (char)i;
        roundTrip("bytes", bytes, 256); }

    {   /*  incompressible: an LCG stream.  Expected to select stored mode.    */
        static char rnd[65536];
        unsigned int seed = 20260803u;
        for (int i = 0; i < (int)sizeof(rnd); i++) {
            seed = seed * 1103515245u + 12345u;
            rnd[i] = (char)((seed >> 16) & 0xFF); }
        roundTrip("random", rnd, (int)sizeof(rnd)); }

    {   /*  the large case: 256K of mixed compressible and incompressible.     */
        static char big[262144];
        const char *phrase = "Buffer sits in the tokenizer's blast path. ";
        unsigned int seed = 7u;
        int at = 0;
        while (at < (int)sizeof(big)) {
            seed = seed * 1103515245u + 12345u;
            if (((seed >> 20) & 3) == 0) {
                big[at++] = (char)((seed >> 8) & 0xFF); }
            else {
                int k = (int)strlen(phrase);
                if (at + k > (int)sizeof(big)) k = (int)sizeof(big) - at;
                memcpy(big + at, phrase, (size_t)k);
                at += k; } }
        roundTrip("large", big, (int)sizeof(big)); }

    /*  SECTION 2 -- refusal.  decompress on ordinary content must diagnose
        and change nothing, rather than guess.                                 */
    {   const char *plain = "this is not a BZ1 stream at all";
        int n = (int)strlen(plain);
        Buffer *b = loaded(plain, n);
        int rc    = b->decompress();
        int kept  = (b->length() == n) && memcmp(b->start, plain, (size_t)n) == 0;
        val("refuse.returned", rc);
        val("refuse.contentKept", kept ? 1 : 0);
        delete b; }

    /*  SECTION 3 -- the mark is cleared by both halves of the pair.  A mark
        points into content that no longer exists after either call.           */
    {   const char *plain = "alpha beta gamma delta alpha beta gamma delta";
        int n = (int)strlen(plain);
        Buffer *b = loaded(plain, n);
        b->setMark();
        b->compress();
        val("mark.afterCompress.markIsSet", b->markIsSet ? 1 : 0);
        val("mark.afterCompress.markNull",  b->mark == 0 ? 1 : 0);
        b->setMark();
        b->decompress();
        val("mark.afterDecompress.markIsSet", b->markIsSet ? 1 : 0);
        val("mark.afterDecompress.markNull",  b->mark == 0 ? 1 : 0);
        delete b; }

    /*  SECTION 4 -- the pair composes.  compress twice, decompress twice,
        back to the original: the encoded form is just content to the next
        compress, which is what makes the registry able to nest envelopes.     */
    {   const char *plain = "nest me nest me nest me nest me nest me nest me";
        int n = (int)strlen(plain);
        Buffer *b = loaded(plain, n);
        int a1 = b->compress();
        int a2 = b->compress();
        int a3 = b->decompress();
        int a4 = b->decompress();
        int same = (b->length() == n) && memcmp(b->start, plain, (size_t)n) == 0;
        val("nest.calls", (a1 && a2 && a3 && a4) ? 1 : 0);
        val("nest.identical", same ? 1 : 0);
        delete b; }

    /*  SECTION 5 -- the LAST section, and the sentinel below it is reachable
        only from here (RULE H2).  A truncated run cannot print it.            */
    {   /*  a buffer whose content is a single byte, and a buffer at the
            match-length boundary, are the two arithmetic edges of the packer. */
        char one[1];
        one[0] = 'x';
        roundTrip("one", one, 1);

        char edge[19];
        for (int i = 0; i < 19; i++) edge[i] = 'q';
        roundTrip("edge", edge, 19);

        printf("SENTINEL bufferPop driver reached the end\n"); }

    return 0;
}
