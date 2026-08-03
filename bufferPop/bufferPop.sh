#!/bin/sh
#  bufferPop -- the isolation POP for Buffer::compress / Buffer::decompress.
#  Run from anywhere:   sh ~/data/support/bufferPop/bufferPop.sh
#
#  WHAT IT PROVES
#    compress and decompress are a SELF-INVERSE PAIR on Buffer: for every
#    fixture, compress -> decompress returns the ORIGINAL BYTES, verbatim,
#    including an embedded zero byte that no Buffer accessor can even express.
#    The charter's three named cases -- round trip, empty buffer, large buffer
#    -- are rows `hello`, `empty` and `large`.
#
#  WHY IT IS A HARNESS AND NOT A SCRIPT (the H-rules, Groups/CLAUDE.md):
#    H1  it ECHOES THE BINARY IT TESTS -- path, size, mtime -- and, because a
#        generated .C that has drifted from its .twk is this project's own
#        recurring lie, it also RETOKS Buffer.twk into a scratch directory and
#        diffs, so a stale Frame/Buffer.C is reported rather than measured.
#    H2  it asserts its own completeness.  The driver prints a SENTINEL that is
#        reachable only through its final section, and THIS SCRIPT CHECKS THAT
#        SENTINEL FIRST AND BY NAME.  A truncated run is uninterpretable, not
#        merely short, so nothing below is read until the sentinel is seen.
#    H3  no assertion moves for a correctness-unrelated reason.  Compression
#        ratios are printed but asserted only as BOUNDS -- an exact byte count
#        would go red every time the packer is tuned, which says nothing about
#        whether the pair is still an inverse.
#    H4  presence-with-value, never absence-of-message.  Every quantity is
#        printed unconditionally by the driver and compared here BY VALUE; a
#        missing line is a FAILURE, so deleting the code that emits it breaks
#        the check instead of satisfying it.
#    H5  every run is under a wall-clock cap.  A hang is not a wrong answer, it
#        is the absence of a run.  A timeout is reported BY NAME and never as a
#        diff, because a killed process yields truncated output and a
#        truncation diff names the wrong row.
#
#  $? is taken directly from the binary via `wait`, NEVER through a pipe --
#  ${PIPESTATUS[0]} is silently empty in zsh and reports every run as passing.

POPCAP=${POPCAP:-90}
SUP=$HOME/data/support
SRC=$SUP/bufferPop
OUT=$SUP/build/bufferPop
BIN=$OUT/bufferPop

pass=0
fail=0

mkdir -p "$OUT" || exit 1
rm -f "$OUT/o.pop" "$OUT/e.pop"

echo "=============================================================="
echo " bufferPop -- Buffer::compress / Buffer::decompress, in isolation"
echo "=============================================================="

# ---------------------------------------------------------------- H1: sources
echo ""
echo "--- H1  the sources under test ---"
for f in "$SUP/Frame/Buffer.twk" "$SUP/Frame/Buffer.C" "$SUP/Frame/Buffer.h" "$SRC/bufferPop.C"; do
    if [ ! -f "$f" ]; then
        echo "  FAIL  missing source $f"
        exit 1
    fi
    ls -l "$f" | awk '{printf "  %9s bytes  %s %s %s  %s\n",$5,$6,$7,$8,$9}'
done

# --------------------------------------------- H1: is the generated .C current?
echo ""
echo "--- H1  Frame/Buffer.C is current with respect to Frame/Buffer.twk ---"
REGEN=$OUT/regen
rm -rf "$REGEN"
mkdir -p "$REGEN" || exit 1
cp "$SUP/Frame/Buffer.twk" "$SUP/Frame/frameIncludes" "$REGEN/" || exit 1
( cd "$REGEN" && tok Buffer.twk ) > "$REGEN/tok.log" 2>&1
tokrc=$?
if [ $tokrc -ne 0 ]; then
    echo "  FAIL  tok Buffer.twk exited $tokrc -- see $REGEN/tok.log"
    fail=$((fail+1))
elif diff -q "$REGEN/Buffer.C" "$SUP/Frame/Buffer.C" > /dev/null 2>&1 \
  && diff -q "$REGEN/Buffer.h" "$SUP/Frame/Buffer.h" > /dev/null 2>&1; then
    echo "  ok    generated .C and .h reproduce byte-identically from the .twk"
    pass=$((pass+1))
else
    echo "  FAIL  Frame/Buffer.C or Frame/Buffer.h has DRIFTED from Buffer.twk"
    diff "$REGEN/Buffer.C" "$SUP/Frame/Buffer.C" | head -20 | sed 's/^/          /'
    fail=$((fail+1))
fi

# ------------------------------------------------------------------- the build
echo ""
echo "--- build ---"
rm -f "$BIN"
clang++ -std=gnu++17 -g -O1 -w \
        -I"$SUP/Frame" -I"$SUP/Maps" \
        -o "$BIN" \
        "$SRC/bufferPop.C" \
        "$SUP/Frame/Buffer.C" \
        "$SUP/Frame/StringRoutines.C" \
        "$SUP/Frame/CharSet.C" > "$OUT/build.log" 2>&1
brc=$?
if [ $brc -ne 0 ] || [ ! -x "$BIN" ]; then
    echo "  FAIL  build exited $brc -- see $OUT/build.log"
    tail -20 "$OUT/build.log" | sed 's/^/          /'
    echo ""
    echo "bufferPOP FAILED -- did not reach the run"
    exit 1
fi
echo "  ok    built (StringRoutines.C and CharSet.C are linked only to satisfy"
echo "        Buffer::flush's checkSys; neither is exercised by this POP)"

# ------------------------------------------------------- H1: echo THE binary
echo ""
echo "--- H1  the binary under test ---"
ls -l "$BIN" | awk '{printf "  %9s bytes  %s %s %s  %s\n",$5,$6,$7,$8,$9}'

# ------------------------------------------------- H5: run under a wall clock
echo ""
echo "--- the run (cap ${POPCAP}s) ---"
#  timeout(1) is not on macOS, so the cap is sleep-and-kill.  The watchdog
#  polls and RETIRES ITSELF once the driver is gone rather than being killed by
#  the script -- killing it makes the shell print a job-control notice into the
#  middle of the log, and a harness that prints noise it cannot explain teaches
#  the reader to skim.
"$BIN" > "$OUT/o.pop" 2> "$OUT/e.pop" &
popPid=$!
(   waited=0
    while [ $waited -lt "$POPCAP" ]; do
        sleep 1
        kill -0 "$popPid" 2>/dev/null || exit 0
        waited=`expr $waited + 1`
    done
    kill -9 "$popPid" 2>/dev/null ) &
wait "$popPid"
rc=$?

if [ $rc -eq 137 ]; then rc=124; fi
if [ $rc -eq 124 ]; then
    echo "  FAIL  TIMEOUT -- the driver did not return inside ${POPCAP}s."
    echo "        Reported by name, NOT as a diff: a killed process yields"
    echo "        truncated output and a truncation diff names the wrong row."
    echo ""
    echo "bufferPOP FAILED -- timeout"
    exit 1
fi
echo "  driver exit status = $rc   (taken directly from the binary, not through a pipe)"

# ---------------------------------------- H2: THE SENTINEL, FIRST AND BY NAME
echo ""
echo "--- H2  completeness ---"
if grep -q '^SENTINEL bufferPop driver reached the end$' "$OUT/o.pop"; then
    echo "  ok    SENTINEL present -- the driver reached its final section"
    pass=$((pass+1))
else
    echo "  FAIL  SENTINEL ABSENT -- the run truncated."
    echo "        Every other line in $OUT/o.pop is uninterpretable, not merely"
    echo "        incomplete, so nothing below it is read.  stderr was:"
    sed 's/^/          /' "$OUT/e.pop" | head -20
    echo ""
    echo "bufferPOP FAILED -- no sentinel"
    exit 1
fi

if [ ! -s "$OUT/o.pop" ]; then
    echo "  FAIL  driver produced no output at all (anti-vacuity guard)"
    fail=$((fail+1))
fi

# ------------------------------------------------------- H4 assertion helpers
val() {
    got=`sed -n "s/^VAL $1 = //p" "$OUT/o.pop"`
    if [ -z "$got" ]; then
        echo "  FAIL  $1 -- NO VALUE PRINTED (H4: an absent line fails, never passes)"
        fail=$((fail+1))
    elif [ "$got" = "$2" ]; then
        echo "  ok    $1 = $got"
        pass=$((pass+1))
    else
        echo "  FAIL  $1 = $got   (expected $2)"
        fail=$((fail+1))
    fi
}

sval() {
    got=`sed -n "s/^STR $1 = //p" "$OUT/o.pop"`
    if [ -z "$got" ]; then
        echo "  FAIL  $1 -- NO VALUE PRINTED (H4)"
        fail=$((fail+1))
    elif [ "$got" = "$2" ]; then
        echo "  ok    $1 = $got"
        pass=$((pass+1))
    else
        echo "  FAIL  $1 = $got   (expected $2)"
        fail=$((fail+1))
    fi
}

# H3: a BOUND, not a byte count.  The value is printed either way.
under() {
    got=`sed -n "s/^VAL $1 = //p" "$OUT/o.pop"`
    if [ -z "$got" ]; then
        echo "  FAIL  $1 -- NO VALUE PRINTED (H4)"
        fail=$((fail+1))
    elif [ "$got" -lt "$2" ]; then
        echo "  ok    $1 = $got  (bound: < $2)"
        pass=$((pass+1))
    else
        echo "  FAIL  $1 = $got  (bound: < $2)"
        fail=$((fail+1))
    fi
}

# every round-trip fixture asserts the same five things, by value
trip() {
    val "$1.compressOK"   1
    val "$1.decompressOK" 1
    val "$1.backLen"      "$2"
    val "$1.rawLen"       "$2"
    val "$1.identical"    1
    val "$1.alphabet"     1
}

echo ""
echo "--- the pair is a self-inverse: compress -> decompress -> ORIGINAL BYTES ---"
trip empty  0
trip hello  5
trip text   2048
trip bytes  256
trip random 65536
trip large  262144
trip one    1
trip edge   19

echo ""
echo "--- the two arithmetic edges of the packer ---"
echo "  (one = a single byte; edge = 19 bytes, one past the 18-byte max match)"

echo ""
echo "--- mode selection: pack when it wins, store when it loses ---"
sval text.mode   z
sval large.mode  z
sval random.mode s
sval bytes.mode  s
sval empty.mode  s

echo ""
echo "--- compression actually compresses (H3: bounds, not byte counts) ---"
under text.pct  40
under large.pct 40
echo "  (measured percentages, reported not asserted:)"
sed -n 's/^VAL \(.*\.pct\) = /          \1 = /p' "$OUT/o.pop"
echo "  NOTE  random.pct and bytes.pct EXCEED 100 by design: stored mode still"
echo "        armours, so incompressible input costs 4/3 plus a 12-byte header."
echo "        The guarantee is a CEILING, not a reduction."

echo ""
echo "--- the encoded form is envelope-safe (this is what Part B rests on) ---"
echo "  every .alphabet row above is 1: the whole compressed form draws only"
echo "  from A-Z a-z 0-9 : - _  -- no NUL, no newline, no quote, no backslash,"
echo "  no space.  It needs no escaping in any text envelope, or in a URL."

echo ""
echo "--- refusal: decompress does not guess ---"
val refuse.returned    0
val refuse.contentKept 1

echo ""
echo "--- both halves clear the mark (it points into content that is gone) ---"
val mark.afterCompress.markIsSet   0
val mark.afterCompress.markNull    1
val mark.afterDecompress.markIsSet 0
val mark.afterDecompress.markNull  1

echo ""
echo "--- the pair composes: compress twice, decompress twice ---"
val nest.calls     1
val nest.identical 1

echo ""
echo "--- driver exit status ---"
if [ $rc -eq 0 ]; then
    echo "  ok    driver exited 0"
    pass=$((pass+1))
else
    echo "  FAIL  driver exited $rc"
    fail=$((fail+1))
fi

# ------------------------------------------------------------- H2: the summary
# This line and the exit status below are reachable only from here.  A run that
# died anywhere above prints no summary at all, which is the signal.
echo ""
echo "=============================================================="
if [ $fail -eq 0 ]; then
    echo "bufferPOP PASSED -- $pass checks, 0 failures"
    echo "=============================================================="
    exit 0
fi
echo "bufferPOP FAILED -- $pass passed, $fail FAILED"
echo "=============================================================="
exit 1
