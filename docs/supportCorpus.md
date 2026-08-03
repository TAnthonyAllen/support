# The support corpus

*What the support minion knows about the support domain. A **sibling** of the JIT's
`jigcorpus` and the `kantCorpus`, living in the support repo, with its own querier —
**not** loaded into `jigcorpus`. Format: `Groups/docs/minion-corpus-format.md` v0.*

```
minion     : Support Minion
domain     : ~/data/support — the shared C++/TAWK support classes consumed by
             PLG, TAWK and Incant. Census unit is the 21 Frame/*.twk plus the
             two C-only entries BaseEntry.C and Stack.C.
lastBaked  : 2026-08-03   (round 2 — TASK 2 PART A: Buffer compress/decompress)
floor      : ~/data/support @ 690dc59ce36f41b86d7f88865f83d58a4b4dd642
             (round 1's census floor — SUP-1..SUP-27 still key to it)
round 2    : ~/data/support @ 1f9f9bf  (the tree round 2 started from, after
             Tony's BeforeRefactor and spent-temporaries rulings landed)
```

⚠ **ROUND 2 IS THE FIRST ROUND THAT CHANGED SOURCE.** SUP-1..SUP-27 are recon and key
to the round-1 floor. SUP-28 onward describe an **edit** to `Frame/Buffer.twk` and key to
round 2's commit. The distinction matters because round 1's claims were graded `inferred`
almost throughout, and round 2's are the corpus's first `verified` claims *about the code*
rather than about the process — they rest on an observed run, not a grep.

**EVERY CLAIM BELOW KEYS TO THAT FLOOR SHA.** It is the TASK 0 snapshot: the working
tree committed verbatim, content marked UNREVIEWED, no edits in the commit. An `asOf`
against an uncommitted tree is an `asOf` against nothing reproducible.

### Confidence, and how this round graded

| value | means |
|---|---|
| `verified` | a **run** was observed |
| `inferred` | read from source, or **measured by grep** — however mechanical |
| `stale-suspect` | believed, now doubted |
| `refuted` | shown false; kept so the trail survives |

⚠ **A grep is `inferred`, not `verified`.** This round is almost entirely greps and
source reads, so it is almost entirely `inferred`. The two `verified` claims are the
git commit and the instrument failure — both things that actually ran. **Nothing in
this corpus has been proven by executing the code it describes.**

⚠ **A CENSUS CLAIM THAT RESTS ON A PROBE REPORTS THE PROBE.** Every count below carries
the command that produced it. A readable command is falsifiable; a confident sentence is
prose. Where a count is *not* trustworthy, the claim says so rather than quoting it.

---

## READ THIS BEFORE TRUSTING ANY NUMBER — THE INSTRUMENT

### CLAIM SUP-1 — the three scopes, and why a single number would be a lie

- **text**: Caller counts differ by 2–3x depending on search scope, and at least one
  class (`Bot`) reads as **completely dead** at the narrow scope and **live** at the wide
  one. Therefore every count in this corpus is recorded at a named scope, and no claim
  quotes a bare number.

  - **NARROW** = `~/data/support` + `InProcess/Groups` — the minion's briefed read scope.
  - **WIDE** = `~/data/support` + all of `InProcess` — the symlink reality.
  - **TRACKED** = WIDE minus gitignored-but-on-disk paths (`Include/TokTests/`,
    `Include/WithJIT/`, `build/`, `DerivedData/`, `Groups/include/` = vendored BDWGC).

  `Frame/`, `Include/` and `KeyTable/` are **symlinked into `InProcess`** and consumed by
  five sibling trees that all hold live source: `Bot` (13 .twk / 13 .mm / 4 .rtn),
  `Groups` (63/28/31 + 4 .C), `Parse` (41 .twk / 15 .C / 5 .rtn), `TOK` (14/7/6 + 4 .m),
  `Tokf` (60 .twk / 34 .C / 5 .rtn), `wbView` (3 .m). `support/CLAUDE.md` states the
  same thing in prose: *"a change to Buffer affects PLG, TAWK, and Incant simultaneously."*

  **Consequence, and it is the reason this claim is first:** a caller count scoped to
  Groups-only reports every Parse/Tokf/TOK/Bot caller as **zero**. That is not a
  conservative error — it is bear-trap #19's corollary exactly, the answer sitting in a
  tree the search never entered. **TRACKED is the scope this corpus recommends**, and the
  scope ruling is parked with Tony (`ipc/support-to-clod.md` SEQ 1, Q1).
- **confidence**: `inferred`
- **provenance**: scope definitions and file-type counts —
  `for d in Bot Groups Parse TOK Tokf wbView Stash; do find $d -name '*.twk' | wc -l; ... done`
  run in `InProcess`, 2026-08-03. Symlinks confirmed by `ls -la ~/…/InProcess` showing
  `Frame -> /Users/anthony/…/data/support/Frame` (likewise `Include`, `KeyTable`).
- **asOf**: 2026-08-03

### CLAIM SUP-2 — ⚠ THE INSTRUMENT LIED FIRST, AND IT LIED IN THE SHAPE OF THE ANSWER

- **text**: The first working version of the census script returned **`0` for every single
  symbol**. Not an error, not a crash — a clean, plausible zero for `Buffer`, `PLGset`,
  `Stak` and `SimpleList` alike. Two causes, both environmental, neither in the code being
  measured:
  1. **The shell is `zsh`, and zsh does not word-split unquoted parameters.** The roots
     were passed as a single space-joined string `roots="$SUP $INP/Groups"`, which zsh
     handed to `grep` as **one** path that does not exist. `ugrep` warned to stderr, which
     was redirected to `/dev/null`, and exited **0**.
  2. **`grep` on this machine is `ugrep`, not GNU grep.**

  **Why this is claim #2 and not a footnote:** the failure produced *zeros*, and zeros are
  precisely what this census was commissioned to find. A wrong count announces itself; a
  wrong **zero** looks exactly like a discovery. Had the run not included `Buffer` — a
  class with 482 references — as an unwitting control, the corpus would have reported the
  entire support tree as dead code, with a recorded command to prove it.

  **Standing lesson: a census must include a symbol whose answer is already known.** The
  anti-vacuity guard (`printPop.sh`'s `if [ ! -s "$T/o.print" ]`) is the same idea one
  level up. *When a result surprises you, doubt the instrument before the code* — and a
  sweeping zero is a surprise, not a finding.
- **confidence**: `verified` — observed in a run, twice, then fixed and re-run.
- **provenance**: first run output `Buffer NARROW=0 WIDE=0 / PLGset 0 0 / Stak 0 0 /
  SimpleList 0 0`; diagnosis `grep -rnw "Buffer" --include='*.twk' $roots` →
  `ugrep: warning: /Users/anthony/data/support /Users/…/Groups: No such file or directory`.
  Corrected instrument (zsh array `roots=($SUP $INP)`) re-run same symbols →
  `Buffer 217/503`, `PLGset 162/441`, `Stak 110/188`, `SimpleList 0/0`.
- **asOf**: 2026-08-03

### CLAIM SUP-3 — generic method names are NOT grep-attributable; zeros are

- **text**: Per-method counts for common names (`add`, `get`, `set`, `next`, `clear`,
  `find`, `remove`, `insert`, `length`, `contains`, `toString`, `pop`, `push`, `prior`)
  are **meaningless as caller counts**. Every list-ish and set-ish class in the ecosystem
  declares them, so a word-grep for `add` returns 1055 TRACKED hits spread across
  `DoubleLinkList`, `HashList`, `BaseHash`, `SearchTree` and GroupItem alike. This corpus
  records those numbers **only as an upper bound**, never as a caller count.

  ⚠ **The asymmetry is the useful half, and it is what makes this census possible at all:
  a HIGH count from a generic name is worthless, but a ZERO from one is airtight.** A
  count can be inflated by collisions; a zero cannot. If the token appears nowhere, no
  amount of name-sharing changes that. So every zero-caller claim below stands at full
  strength regardless of how generic the name is, and every large number below is
  explicitly *not* load-bearing.

  Distinctive names (`getMarkedString`, `popOff`, `addSegment`, `urlDecode`,
  `getURLintoBuffer`, `generateNamed`, `removeFromHash`) grep cleanly in both directions.
- **confidence**: `inferred`
- **provenance**: `methodsOf()`/`report()` in the round-1 scratch script, run over all 23
  census entries 2026-08-03. Illustrative: `add` TRACKED=1055, `get`=1479, `next`=1349,
  `length`=1160 — all far exceeding any one class's plausible use.
- **asOf**: 2026-08-03

---

## THE CENSUS

### CLAIM SUP-4 — the census unit, inventoried

- **text**: `Frame/` holds exactly **21 `.twk`** (the source-of-truth set) plus the two
  C-only entries. Per-file shape at the floor:

  | file | .twk lines | generated | .h decls | in support.xcodeproj Sources |
  |---|---|---|---|---|
  | BaseHash | 240 | `.C` | 14 | yes |
  | Bot | 33 | `.mm` | 2 | **no** |
  | Buffer | 495 | `.C` | 30 | yes |
  | CharSet | 409 | `.C` | 21 | yes |
  | DispatchQ | 111 | `.C` | 10 | yes |
  | DoubleLink | **2** | `.C` | 8 | yes |
  | DoubleLinkList | 45 | `.C` | 27 | yes |
  | Hasher | 132 | `.C` | 7 | yes |
  | HashLink | 133 | `.C` | 9 | yes |
  | HashList | 437 | `.C` | 26 | yes |
  | OCroutines | 58 | `.mm` | 3 | **no** |
  | PLGset | 447 | `.C` | 26 | yes |
  | SearchItem | 15 | `.C` | 1 | yes |
  | SearchNode | 42 | `.C` | 2 | yes |
  | SearchTree | 211 | `.C` | 7 | yes |
  | **SimpleList** | 84 | **none** | — | **no** |
  | Stak | 147 | `.C` | 10 | yes |
  | StringRoutines | 574 | `.C` | 30 | yes |
  | Tape | 161 | `.C` | 9 | yes |
  | TapeSegment | 42 | `.C` | 3 | yes |
  | URLservice | 68 | `.mm` | (ObjC `@interface`) | **no** |
  | *BaseEntry* (C-only) | — | 55-line `.C` | 4 | yes |
  | *Stack* (C-only) | — | 119-line `.C` | 8 | yes |

  Two shape notes: **`DoubleLink.twk` is 2 lines** — two `include` directives; the real
  body is `DoubleLink.rtn` (121 lines). Same pattern for `DoubleLinkList.rtn` (427) and
  `HashList.rtn` (434). And the **three `.mm` files are not in `support.xcodeproj`'s
  Sources phase** — yet `OCroutines` has 48 TRACKED references, so it is compiled by some
  *other* target. `support.xcodeproj` is not the whole build.
- **confidence**: `inferred`
- **provenance**: `wc -l` per `.twk`; `grep -c ');$' <cls>.h`;
  `grep -o '/\* [A-Za-z]*\.\(C\|mm\|c\) in Sources \*/' support.xcodeproj/project.pbxproj | sort -u`
  → 24 compiled files (19 from Frame, 2 from Maps, 3 from KeyTable). All 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-5 — class-level reference counts, all three scopes

- **text**: Counts exclude the class's own `Frame/<cls>.*` files, `Frame/BeforeRefactor/`
  and `Include/BackupIncludes/`. **Per SUP-3 these are upper bounds, not caller counts** —
  their value is the *shape*, especially the zeros and the NARROW/WIDE gaps.

  | class | NARROW | WIDE | TRACKED |
  |---|---|---|---|
  | Buffer | 217 | 503 | **482** |
  | PLGset | 162 | 441 | 418 |
  | DoubleLink | 233 | 341 | 313 |
  | DoubleLinkList | 117 | 225 | 193 |
  | Stak | 110 | 188 | 165 |
  | BaseHash | 81 | 205 | 148 |
  | HashLink | 136 | 136 | 136 |
  | StringRoutines | 44 | 89 | 75 |
  | Tape | 31 | 58 | 58 |
  | OCroutines | 32 | 51 | 48 |
  | CharSet | 24 | 32 | 32 |
  | SearchItem | 25 | 25 | 25 |
  | SearchNode | 24 | 24 | 24 |
  | DispatchQ | 10 | 15 | 15 |
  | TapeSegment | 12 | 12 | 12 |
  | **Bot** | **0** | **11** | **11** |
  | Hasher | 10 | 10 | 10 |
  | HashList | 8 | 8 | 8 |
  | SearchTree | 6 | 9 | 7 |
  | **Stack** | 5 | 5 | **2 (all false positives — see SUP-8)** |
  | **BaseEntry** | 14 | 14 | **0 (all hits gitignored — see SUP-9)** |
  | **SimpleList** | **0** | **0** | **0** |
  | **URLservice** | **0** | **0** | **0** |

  **`Bot` is the proof that scope matters**: 0 NARROW, 11 WIDE, every hit in
  `InProcess/Bot/`. Under the briefed scope it would have been reported dead. It is not.
- **confidence**: `inferred`
- **provenance**: `count()` helper —
  `grep -rnw "$sym" --include='*.twk' --include='*.C' --include='*.mm' --include='*.h' --include='*.rtn' --include='*.m' --include='*.c' "${roots[@]}" | grep -v '/Frame/BeforeRefactor/' | grep -v '/Include/BackupIncludes/' | grep -v "/Frame/${own}\."`
  with TRACKED additionally filtering `-e '/Include/TokTests/' -e '/Include/WithJIT/' -e '/build/' -e '/DerivedData/' -e '/Groups/include/'`. Run 2026-08-03.
- **asOf**: 2026-08-03

---

## ZERO-CALLER FINDINGS

> ⚠ **RULE 3, STANDING: A ZERO-CALLER RESULT IS A GREP RESULT, NOT A DELETION LICENCE.**
> Everything in this section is a **candidate**. *Unused* and *safe to remove* are
> different claims, and only Tony signs the second one. Parking is Tony's signature.
>
> Every entry below was verified by an **unfiltered** search — no `--include` list at all,
> both trees, every file type — so the absence claim names its search per format RULE 1.
> Prose mentions (`.md`, `TODO`) and Xcode `project.pbxproj` file-references are reported
> separately from code references, because neither is a caller.

### CLAIM SUP-6 — `SimpleList.twk` is fully isolated: no callers, no build, no declaration

- **text**: The pre-registered hypothesis **holds, and is stronger than predicted.**
  `SimpleList` has zero code references at all three scopes, and three further facts each
  independently confirm the isolation:
  1. It is the **only** census file with **no generated `.C`/`.mm` and no `.h`**.
  2. It is **not in any Sources build phase** — `support.xcodeproj` carries only a
     `PBXFileReference`, never a `PBXBuildFile`. It is listed, not compiled.
  3. It is **declared in no `Include/` manifest whatsoever** — unlike every other live
     class, it has no `external SimpleList` block anywhere, so no TAWK source could
     reference it even if it wanted to.

  Outside its own `.twk` it appears **only** in four Xcode project files. Its whole
  content is a 84-line resizing `void**` list (`clear`/`get`/`push`/`pop`/`resize`).
  The 2026-05-28 recon reached the same conclusion by a different route ("`push(void*)`
  only. Internal.").

  **Strongest dead candidate in the census.** Note also `pop()` returns an uninitialised
  `void *top;` when the list is empty — a real defect, recorded here rather than fixed
  (no source edits during recon).
- **confidence**: `inferred`
- **provenance**: `grep -rlw SimpleList ~/data/support <InProcess>` (no `--include`,
  excluding `BeforeRefactor`, `BackupIncludes`, `.git`, and this corpus' own docs) →
  `Frame/SimpleList.twk`, `support.xcodeproj/project.pbxproj`, and three
  `TOK.xcodeproj/project.pbxproj*` files. Build phase: `grep -n SimpleList
  support.xcodeproj/project.pbxproj` → lines 91, 192 only, both `PBXFileReference`/group
  membership, no `in Sources`. Manifest: `grep -rn 'SimpleList' ~/data/support/Include/`
  → no output. Source read `Frame/SimpleList.twk`. All 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-7 — `URLservice` has zero callers and is not compiled

- **text**: Zero code references at all three scopes. Outside its own three files
  (`.twk`/`.mm`/`.h`) it appears only in Xcode project files and two Groups design docs
  (`docs/gui.md`, `docs/bot-recon.md` — prose, not callers). `URLservice.mm` is **not in
  `support.xcodeproj`'s Sources phase**.

  It is an ObjC `@interface` wrapping `NSURLConnection` (async send + four delegate
  callbacks). The 2026-05-28 recon listed `send(String url)` as "plausible… limited value
  without callback access". **Second-strongest dead candidate** — but note it is the only
  networking code in the support tree, and TASK 2's registry work has "a future web
  channel behind it", so this is a *park-aside* candidate rather than a delete candidate.
  A source read also shows `connectionDidFinishLoading` testing `if file == filename`
  where `file` is never declared in the class — it would not survive a compile, which is
  consistent with it never being compiled.
- **confidence**: `inferred`
- **provenance**: unfiltered `grep -rlw URLservice` over both trees (same exclusions as
  SUP-6) → `Frame/URLservice.{h,mm,twk}`, 4 `project.pbxproj*`, `Groups/docs/gui.md`,
  `Groups/docs/bot-recon.md`. Sources phase: `URLservice` absent from
  `grep -o '/\* [A-Za-z]*\.\(C\|mm\|c\) in Sources \*/' support.xcodeproj/project.pbxproj`.
  Source read `Frame/URLservice.twk`. 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-8 — ⚠ `Stack.C` vs `Stak.twk` — ✅ **THE COLLISION IS DELETED 2026-08-03**

- ✅ **RULING (Tony, 2026-08-03): `Stack.C` is a spent temporary, same can as `Buffer.rvsd`.**
  Removed: `Frame/Stack.C`, `Frame/Stack.h`, **and its 4 entries in
  `support.xcodeproj/project.pbxproj`** — it was wired into the Sources build phase, so a
  file-only delete would have broken that project. Verified after: incant rebuilt **BUILD
  SUCCEEDED**, jitLadder 83 exit 0, `oneTest` exit 0, `pop.sh` 32 green.

  ✅ **The near-collision this claim exists to warn about is now GONE BY CONSTRUCTION** — there is
  no `Stack` left to misgrep against `Stak`. The warning is kept as the reasoning trail.

  ⚠ **ONE THING DELIBERATELY NOT DONE, and it is flagged rather than swept:** `Include/frame`
  still carries `external Stack` and `external StackItem` blocks (lines 4, 426, 434). They are
  **inert** — a declaration with no definition and, per this corpus, **zero callers** — so they
  break nothing. They were left because `Include/frame` is **shared with PLG and TAWK**, whose
  builds cannot be tested from here, and because that file is a bear-trap #16 hand-sync target
  where *this corpus itself* found an anchored grep missing 26 of 33 declarations. **Removing them
  is a separate, cross-project act needing a build of the other two projects.**

- **text (as raised)**: the near-collision was real, and it already produced false positives in
  this very census

- **text**: **Two different classes, one letter apart, both live in `Frame/`.**
  - `Stak` (`.twk`, 147 lines) — the resizing `void**` stack, **165 TRACKED references**,
    declared in both `Include/frame` and `Include/groups.ext`, incant-visible.
  - `Stack` (`.C` only, 119 lines, no `.twk`, hand-written) — a linked-list stack/queue
    with a nested `StackItem`, **zero real callers**.

  The charter predicted this would be "a future misgrep waiting to happen". **It did not
  wait.** `Stack` returned 5 WIDE / 2 TRACKED hits, and inspection shows **every one is a
  false positive** — the English word "Stack" in comments:
  `Groups/jitContext.h:34` *"Stack of pending endif merge blocks"*, `:41` *"Stack of
  pending else blocks"*, and three vendored BDWGC comments in `Groups/include/private/`.
  Not one is a reference to the class. **The class `Stack` has zero callers**, and its two
  distinctive methods `getTop()` and `popFront()` have zero references anywhere outside
  its own `.C`/`.h` and `Include/frame`.

  ⚠ **Two traps, and the second is worse than the first.** (1) A case-insensitive or
  substring grep for `Stak` also matches `Stack` — so a sloppy search makes the dead class
  look alive by borrowing the live one's 165 hits. (2) `Stack` **is compiled** (it is in
  the Sources phase), so it costs build time and links in, while being reachable only
  through an `external Stack` declaration nobody uses. **Dead-but-compiled is the worst
  state to be in**, because neither the build nor the linker will ever complain.

  Recorded as its own claim per the charter, which is exactly where such a thing gets named.
- **confidence**: `inferred`
- **provenance**: `hits Stack wide Stack` → 5 lines, all read individually and all
  comments. `grep -rlw getTop` / `popFront` (unfiltered, both trees) →
  `Include/frame`, `Frame/Stack.C`, `Frame/Stack.h` only. Sources phase: `grep -c
  "Stack.C in Sources" support.xcodeproj/project.pbxproj` → nonzero. `Stak` TRACKED=165
  by the SUP-5 command. 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-9 — `BaseEntry` is compiled, and its only consumers are gitignored

- **text**: `BaseEntry` shows **14 hits at WIDE and 0 at TRACKED**, because *every* hit is
  inside `~/data/support/Include/TokTests/` — a directory that is **on disk but excluded by
  `.gitignore`** (`Include/TokTests/`). Consumers there are `FormatC.C`, `SymbolType.C`,
  `SymbolList.C`, `InstanceTable.C`, `Types.C`, `Tawk.C`.

  So `BaseEntry.C` is **compiled into the support target** while being referenced only by
  code that is not in the repository. This is the sharpest illustration of why the
  TRACKED scope had to exist: WIDE says "14 callers, healthy", TRACKED says "zero callers
  in any tracked source". **Both are true and they mean opposite things.**

  Which answer counts depends on whether `Include/TokTests/` is live scratch or abandoned
  — that is Tony's call, and it is the same shape of question as `BeforeRefactor/`.
  `SearchTree` is a milder case of the same thing (7 TRACKED, of which the real consumers
  are `Tokf/FormatC.C` plus TokTests copies).
- **confidence**: `inferred`
- **provenance**: `hits BaseEntry wide BaseEntry` → 14 lines, all under
  `/Users/anthony/data/support/Include/TokTests/Links/`. `.gitignore` line
  `Include/TokTests/` read directly. TRACKED count 0 by the SUP-5 command. 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-10 — eight `StringRoutines` externs have zero callers

- **text**: `StringRoutines` exposes 30 `extern "C"` free functions. Because they are free
  functions with distinctive names, a word-grep **is** attributable here (SUP-3), making
  this the most trustworthy per-entry-point measurement in the census. Eight have zero
  references outside `Frame/StringRoutines.{twk,C,h}` and the `Include/frame` declaration:

  `alignRIGHT` · `convertNewLine` · `headToChar` · `headToSet` · `stringIsNumeric` ·
  `toStringFromFloat` · `toStringFromLong` · `urlDecode`

  Live for contrast (TRACKED): `compare` 291, `concat` 140, `indent` 116, `tail` 81,
  `toStringFromInt` 62, `getStringFromFile` 55, `bitCount` 47, `alignLEFT` 14,
  `headToCount` 11 — though `compare`/`tail`/`indent`/`concat` are generic names and
  inflated per SUP-3.

  Note the shape: `alignLEFT` is used 14 times and its sibling `alignRIGHT` never;
  `toStringFromInt` 62 times and `toStringFromLong`/`FromFloat` never. **These are
  completed-symmetry functions** — written because the set looked incomplete, not because
  a caller wanted them. That is a different kind of dead code from an abandoned feature,
  and a reasonable argument for keeping them.
- **confidence**: `inferred`
- **provenance**: per-function `count()` at all three scopes (SUP-5 command, `own=StringRoutines`),
  then unfiltered confirmation `grep -rlw <fn> ~/data/support <InProcess>` for each of the
  eight → in every case exactly `Include/frame`, `Frame/StringRoutines.h`,
  `Frame/StringRoutines.twk`, `Frame/StringRoutines.C`. 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-11 — `Buffer`'s mark-vocabulary is half-built, and TASK 2 needs to know which half

- **text**: `Buffer` is the most-used class in the census (482 TRACKED). Its **mark**
  vocabulary — the thing the 2026-05-28 recon called the headliner for incant wrapping —
  is **partially dead**:

  | method | TRACKED | note |
  |---|---|---|
  | `setMark` | 31 | live |
  | `unMark` | 14 | live |
  | `getMarkedString` | 8 | live |
  | `backupToMark` | 2 | barely live |
  | **`insertAtMark`** | **0** | **zero callers** |
  | **`shiftAtMark`** | **0** | **zero callers** |
  | **`appendLong`** | **0** | **zero callers** |
  | `insertIntoBuffer` | 8 | live |
  | `deleteFromBuffer` | 6 | live |
  | `findInBuffer` | 6 | live |
  | `appendString` | **1003** | the hot path |
  | `tabRight` | 2 | barely live |

  ⚠ **`insertAtMark` has zero callers but is NOT a deletion candidate**, and this is the
  clearest case in the census of why rule 3 exists. The 2026-05-28 recon nominated
  `insertAtMark(GroupItem)` as one of the **five methods most worth wrapping for incant**,
  and `IncantForms/Notions/flags:16` records the wish in Tony's own words: *"Methinks
  would be nice as an incant extern method like insertAtMark(GroupItem field)."* It is
  **not abandoned — it is built ahead of its callers.** A census that reported only the
  number would have nominated for deletion the exact method the next task wants.

  This is the general warning for TASK 2: **in a tree that is about to grow, a zero-caller
  count cannot distinguish "dead" from "not yet used".** Only intent can, and intent is
  not greppable.
- **confidence**: `inferred`
- **provenance**: per-method `count()` (SUP-5 command, `own=Buffer`). Unfiltered
  confirmation for the three zeros → own files + `Include/frame` + prose only:
  `insertAtMark` also in `support/TODO.md`, `Tokf/TODO.md`, `Parse/TODO.md`,
  `Groups/XML/Notions/flags:16`, `Groups/IncantForms/Notions/flags:16` (all prose);
  `shiftAtMark` also in `Groups/projectBible.md`, `Groups/TODO.md` (prose).
  Recon cross-reference: `Groups/docs/support-extern-recon-2026-05-28.md` lines 21–29, 101–111.
- **asOf**: 2026-08-03

### CLAIM SUP-12 — four more distinctive zero-caller methods

- **text**: Each has a distinctive name, so each zero is airtight:
  - **`Stak::popOff(int)`** — 0 everywhere. *Nominated by the 2026-05-28 recon for incant
    wrapping* ("Drop N items at once"), so same caveat as `insertAtMark`.
  - **`Tape::addSegment()`** — 0 everywhere. Internal to the allocator; `getStrip()` (23)
    is the live entry point.
  - **`Stack::getTop()` / `Stack::popFront()`** — 0 everywhere (see SUP-8).
  - **`BaseHash::listKeys()` and `removeFromHash()`** — 0 NARROW, 2 TRACKED each.
  - **`PLGset::generateNamed()`** 0 NARROW / 5 TRACKED, **`PLGset::toText()`** 0 NARROW /
    6 TRACKED — both invisible at the briefed scope, both live at the real one. More
    SUP-1 evidence.
- **confidence**: `inferred`
- **provenance**: per-method `count()` (SUP-5 command); unfiltered `grep -rlw` for
  `popOff`, `addSegment`, `getTop`, `popFront` → own class files + `Include/frame` only.
  2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-13 — the hash cluster refers only to itself

- **text**: `HashList` (8 TRACKED), `HashLink` (136), `Hasher` (10) form a **closed
  triangle**: reading the hits individually, every `HashList` reference is in
  `Frame/HashLink.{twk,C,h}`, and every `Hasher` reference is in
  `Frame/HashList.{rtn,twk,C,h}` and `Frame/HashLink.C`. **No file outside the
  `Frame/Hash*` cluster references any of them.** `HashLink`'s larger 136 is its own
  header/`.C`/`.twk` cross-talk plus generic-name collisions (`append`, `insert`, `remove`).

  So this is a **three-class subsystem with zero external consumers** — 702 lines of
  `.twk` plus 434 lines of `.rtn`. The live hash path in the ecosystem is `BaseHash`
  (148 TRACKED) and GroupItem's own registry layer, not this. Cluster-level park-aside
  candidate; the 2026-05-28 recon independently called it "internal hash plumbing".

  ⚠ Cluster claims need cluster-level care: removing any one of the three breaks the other
  two, and the zero is only meaningful for **the set**, not for its members.
- **confidence**: `inferred`
- **provenance**: `hits HashList wide HashList` → 8 lines, all `Frame/HashLink.*`.
  `hits Hasher wide Hasher` → 10 lines, all `Frame/HashList.*` + `Frame/HashLink.C`.
  Both read line by line, 2026-08-03.
- **asOf**: 2026-08-03

---

## STRUCTURE, MANIFESTS AND HAZARDS

### CLAIM SUP-14 — the incant-visible support surface is exactly three classes

- **text**: `Include/groups.ext` — the manifest that makes a C++ class visible to the
  Incant build — declares **only three** support classes: **`PLGset`, `Buffer`, `Stak`**
  (lines 6, 9, 12). The other 18 census files are **invisible to incant**.

  `Include/frame` (589 lines) carries the fuller TAWK-side mirror with member blocks for
  `DoubleLink`, `DoubleLinkList`, `CharSet`, `Stack`, `Stak`, `Tape`, `BaseHash`,
  `Buffer` (line 41), `GroupItem`, `DispatchQ`, `HashList`, `HashLink`, `Hasher`,
  `KeyTableItem`, `KeyMap`, `KeyTable`, `PrintBuffer`, `SearchItem`, `SearchNode`,
  `SearchTree`, `StackItem`, `StringBuffer`, `StringRoutines`, `TapeSegment`.

  **This is the single most useful structural fact for the kant question (SUP-21).**
  The support/incant boundary is not diffuse — it is three names in one file.
- **confidence**: `inferred`
- **provenance**: `grep -nE '^external[[:space:]]+[A-Za-z]' Include/frame` (33 lines) and
  the same over `Include/*.ext`, `globals`, `OCframe`, `tok.ext`, `PLGrevision`,
  `GUIexternals`, `maps`. Result set: `groups.ext:6,9,12`; `frame:1–562`;
  `PLGrevision:5,11,309`; `OCframe:276`. 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-15 — ⚠ an anchored grep MISSES declarations in these manifests

- **text**: `Include/frame` uses **inconsistent whitespace** after `external` — tabs,
  single spaces and four spaces all appear. `grep -nE '^external [A-Za-z]'` (single
  space) finds **7** declarations; `grep -nE '^external[[:space:]]+[A-Za-z]'` finds **33**.

  **`Buffer`'s own external block was in the 26 that the first pattern missed**
  (`Include/frame:41`, `external    Buffer`) — i.e. the naive pattern silently concluded
  that the most-used class in the census had no TAWK mirror at all.

  This matters beyond tidiness: `Include/frame` and `groups.ext` are the **bear-trap #16
  sync targets** — the mirrors that must be edited by hand when a class's members change,
  and that `tok` merges against rather than regenerates. **Anyone auditing that sync with
  an anchored single-space grep will get a false all-clear.** Use `[[:space:]]+`.
- **confidence**: `inferred`
- **provenance**: both greps run against `~/data/support/Include/frame` 2026-08-03 →
  7 vs 33 lines; `Buffer` present only in the second (`41:external    Buffer`).
- **asOf**: 2026-08-03

### CLAIM SUP-16 — two orphan `external` declarations name classes that do not exist

- **text**: `Include/frame` declares **`PrintBuffer`** (line 380, with constructors
  `PrintBuffer(int)`, `PrintBuffer(String,int)`) and **`StringBuffer`** (line 476, with
  `StringBuffer()`, `StringBuffer(int)`). An unfiltered search of **both trees** finds
  these names in **`Include/frame` and nowhere else** — no `.twk`, no `.C`, no `.h`, no
  caller, no definition.

  These are declarations of classes that do not exist. They are harmless while nothing
  references them (a TAWK `external` block is a promise, not a link), but they are
  **actively misleading to a reader auditing the manifest**, and they would produce an
  unresolved-symbol failure the moment someone took the declaration at its word.

  The other six "orphans" from the same sweep are legitimate: `gc` (Boehm GC, external
  library), `GroupItem` (lives in `Groups/`), `KeyTable`/`KeyMap`/`KeyTableItem` (live in
  `support/KeyTable/`), `StackItem` (nested inside `Stack.h`). Only `PrintBuffer` and
  `StringBuffer` have no home anywhere.
- **confidence**: `inferred`
- **provenance**: orphan sweep — for each name from
  `grep -oE '^external[[:space:]]+[A-Za-z_][A-Za-z0-9_]*' Include/frame | awk '{print $2}'`,
  test `[ ! -f Frame/$n.twk ] && [ ! -f Frame/$n.C ] && [ ! -f Frame/$n.mm ] && [ ! -f Frame/$n.h ]`
  → 8 orphans. Then `grep -rlw PrintBuffer` / `StringBuffer` over `~/data/support` and
  `InProcess` (unfiltered, minus `BeforeRefactor`/`BackupIncludes`/`.git`) → one file each,
  `Include/frame`. 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-17 — `Frame/BeforeRefactor/` was an exact mirror of the entire census unit — ✅ **RULED AND ARCHIVED 2026-08-03**

*(raised by this minion as the charter's one permitted claim on the directory; ruled by Tony
the same day and executed by Clod, since it mutates the repo the minion only reads)*

- ✅ **RULING (Tony, 2026-08-03): archaeology going stale. Tarred, stashed, deleted.**

  ```
  ~/data/attic/support-BeforeRefactor-b9aae1a-2026-08-03.tar.gz          36058 bytes
  ~/data/attic/support-BeforeRefactor-b9aae1a-2026-08-03.MANIFEST.txt    24 files, sha256 each
  ```

  **Stash is `~/data/attic/` — deliberately OUTSIDE the support repo**, so the tarball is
  not a binary blob in git and **cannot be re-censused**. `~/data` symlinks to
  `Dropbox/data`, so it is backed up.

  **Cut against git history:** source commit `b9aae1a` recorded in the tarball name *and*
  in the manifest, along with the floor SHA `690dc59` (content byte-identical in both —
  the corpus commit did not touch it). **All 24 files were tracked at deletion, so git is
  still a full second copy**; the tarball is convenience, not the only one. Recovery
  commands are in the manifest.

  **Verified before deleting** — the tarball was extracted to a scratch dir, `diff -r`
  against the original reported **identical**, and all **24 manifest hashes verified
  against the extracted copy** (not against the original, which would have proved
  nothing). *An archive nobody extracted is a claim, not a backup.*

  ✅ **The operational consequence below is now CLOSED BY CONSTRUCTION**, which was the
  point of the ruling: `find ~/data/support -name 'Buffer.twk'` returns **1**, not 2. A
  future census cannot silently double, because there is nothing left to double against —
  the `grep -v '/Frame/BeforeRefactor/'` filters in this corpus are now belt-and-braces
  rather than load-bearing.

- **text (as raised)**: It existed, was **not gitignored** (committed at the floor SHA), and
  awaited Tony's ruling on whether it was archaeology like `Aside/` or live.

  ⚠ **It is 24 files, not the 5 the charter's margin note describes.** It holds a
  **name-for-name copy of all 21 census `.twk`** — `diff <(ls BeforeRefactor/*.twk) <(ls
  Frame/*.twk)` reports the name sets **identical** — plus `DoubleLink.rtn`,
  `DoubleLinkList.rtn`, `HashList.rtn` (which also exist in `Frame/` proper). The "5" in
  the margin note was the count of files *modified in the working tree*, which is a
  different fact from the directory's size; 7 of the 24 were dirty at the floor.

  ⚠ **Operational consequence, and it is why this claim is not merely archival: every grep
  for any Frame class name hits this directory and silently doubles the result.** The
  names match exactly, so nothing about the output looks wrong. Every command recorded in
  this corpus excludes it by path (`grep -v '/Frame/BeforeRefactor/'`), and any future
  census must do the same. A doubled count here would read as healthy usage.

  Not resolved by this minion. Flagged to Tony (`ipc/support-to-clod.md` SEQ 1, Q2) with
  the note that the ruling covers 5x more material than it appeared to. **That flag is what
  produced the ruling the same day** — and the "5x more material" line is why it was ruled
  rather than deferred.
- **confidence**: `verified` (the ruling and the archive were run; the mirror measurement
  that prompted them remains `inferred`, being a set of `ls`/`diff` reads)
- **provenance**: `ls Frame/BeforeRefactor/*.twk | wc -l` → 21; `ls *.rtn | wc -l` → 3;
  `ls | wc -l` → 24; `diff <(ls BeforeRefactor/*.twk|xargs -n1 basename) <(ls Frame/*.twk|xargs -n1 basename)`
  → no differences. Dirty count from `git status --porcelain` before the TASK 0 commit.
  2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-18 — five source files hardcode `/Users/anthony/Dropbox`, which is a symlink

- **text**: `Frame/frameIncludes`, `Frame/PLGset.twk`, `Frame/DoubleLink.twk`,
  `Frame/BaseHash.twk` and `Frame/CharSet.twk` contain absolute `include` paths under
  `/Users/anthony/Dropbox/data/InProcess/…`. That path is **not** where the files live —
  it resolves only because `/Users/anthony/Dropbox` is a **symlink** to
  `/Users/anthony/Library/CloudStorage/Dropbox`, created 2025-03-11.

  It works today (verified by `ls` through the path). It is recorded because the whole
  build of five source-of-truth files depends on one undocumented symlink in the home
  directory, and because `DoubleLink.twk` is **nothing but** two such includes — its
  entire content is an absolute path and a relative one, so it fails hardest if the
  symlink goes.

  Same family as bear-trap #11 (`groups.ext` lives outside the repo): a real build
  dependency that no repo tracks and no `git status` will ever mention.
- **confidence**: `inferred`
- **provenance**: `grep -rln '/Users/anthony/Dropbox' ~/data/support/Frame ~/data/support/Include`
  (minus `BeforeRefactor`/`BackupIncludes`) → the five files above plus four under
  `Include/TokTests/` (gitignored). `ls -la /Users/anthony/Dropbox` →
  `lrwxr-xr-x … -> /Users/anthony/Library/CloudStorage/Dropbox` (Mar 11 2025).
  `ls -la /Users/anthony/Dropbox/data/InProcess/Include/frame` → resolves. 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-19 — `Frame/Buffer.rvsd` — ✅ **RULED SPENT AND DELETED 2026-08-03**

- ✅ **RULING (Tony, 2026-08-03):** *"I make those files and forget them after incorporating
  whatever I wanted to incorporate."* A **spent temporary**, not a live alternate. **Deleted**
  (`Frame/Buffer.rvsd`), along with `Frame/Stack.C` and `Frame/Stack.h` under the same word.

  **No tarball — deliberately, and the ruling permits it** (*"tarball optional at Clod's
  discretion"*). The file was **tracked** as of the floor snapshot, so git is a complete copy:
  `git -C ~/data/support show 690dc59:Frame/Buffer.rvsd`. Attic-ing something *ruled spent* would
  contradict the very rule the ruling established — **attic is for uncertain, not for spent.**

  ⚠ **THIS UNBLOCKS TASK 2 AND WAS HALF THE POINT:** with `.rvsd` ruled spent, **the Buffer base
  TASK 2 builds on is unambiguous — what is in the tree is the truth.** This claim was the open
  question sitting directly in TASK 2's path; it is now closed rather than carried.

- **text (as raised)**: `Frame/Buffer.rvsd` was an unreviewed alternate revision of the TASK 2
  headliner

- **text**: `Frame/Buffer.rvsd` (10,370 bytes, dated 2026-05-08) was **untracked** until
  the TASK 0 snapshot. It is a substantially reworked `Buffer.twk`: `diff Buffer.twk
  Buffer.rvsd` produces **402 lines**. It drops the `markIsSet` boolean and the
  constructors as written, and introduces at least one method with no counterpart in the
  current class (`appendCount(int, String format)`).

  ⚠ **This is directly in TASK 2's path.** TASK 2 adds compress/decompress and a registry
  to `Buffer` — the same file — under a standing bear-note that Buffer sits in the
  tokenizer's blast path. Starting that work without knowing whether `Buffer.rvsd` is
  abandoned, in-progress, or the intended next version risks either duplicating it or
  silently reverting it.

  **Nobody should assume the floor snapshot reviewed it.** The TASK 0 commit explicitly
  marks its content UNREVIEWED, and this file is a large part of why that mattered.
  `Frame/Buffer.new` (2020-10-25) and `Frame/{BaseHash,DoubleLinkList}.save` (2020-03-26)
  are older files of the same kind and were already tracked.
- **confidence**: `inferred`
- **provenance**: `ls -la Frame/Buffer.rvsd` → 10370 bytes, May 8 15:55.
  `diff Frame/Buffer.twk Frame/Buffer.rvsd | wc -l` → 402. First 40 diff lines read
  directly. Untracked status from `git status --porcelain` before the TASK 0 commit
  (`?? Frame/Buffer.rvsd`). 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-20 — the floor snapshot committed a file whose own header says "Not for repo"

- **text**: `Frame/devDirectives` (7,386 bytes, 2026-05-28) was untracked and is now
  committed at the floor SHA. Its own second line reads: *"devDirectives — scratch
  workspace for incant development directives. **Not for repo.** Hand-edited workspace for
  trying directive shapes."*

  This is reported rather than corrected. TASK 0's instruction was to commit the tree
  **verbatim with no edits**, and adding a `.gitignore` entry or omitting the file would
  have been an edit — precisely the laundering the snapshot exists to prevent. The commit
  message marks all content UNREVIEWED, which covers it.

  **Owed to Tony, not to this minion:** if `devDirectives` should not be tracked, the fix
  is a follow-up commit, and it should be a deliberate one. Same for
  `support.xcodeproj/xcuserdata/` (Xcode per-user state) and `incantConfig.json`, both
  also swept in verbatim.
- **confidence**: `verified` — the commit ran; the file's text was read directly.
- **provenance**: `head -25 Frame/devDirectives`; `git status --porcelain` pre-commit
  showing `?? Frame/devDirectives`, `?? support.xcodeproj/xcuserdata/`,
  `?? incantConfig.json`; floor commit `690dc59`. 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-21 — `KeyTable/` and `Maps/` are outside the census unit but inside the domain

- **text**: The charter's census unit is the 21 `Frame/*.twk` plus `BaseEntry.C` and
  `Stack.C`. Two further content directories were therefore **not censused** and carry no
  caller counts in this corpus:
  - `KeyTable/` — `KeyMap`, `KeyTable`, `KeyTableItem` (3 classes, `.twk`/`.C`/`.h` each,
    all three **compiled** in `support.xcodeproj`, all three declared in `Include/frame`).
    `KeyMap.{twk,C,h}` and `KeyTable.{twk,C,h}` were among the floor's dirty files.
  - `Maps/` — `BitMAP`, `Segment` (`.twk`/`.C`/`.h` each, both compiled), plus
    `perfect.c` and the `maps` manifest. Symlinked into `Groups/Maps`.

  Recorded so the boundary is explicit: **their absence from this corpus is a scope
  decision, not a finding.** Nothing here says they are unused; nobody looked.
- **confidence**: `inferred`
- **provenance**: `ls ~/data/support/KeyTable ~/data/support/Maps`; Sources-phase list
  from `grep -o '/\* [A-Za-z]*\.\(C\|mm\|c\) in Sources \*/' support.xcodeproj/project.pbxproj | sort -u`
  → includes `BitMAP.C`, `Segment.C`, `KeyMap.C`, `KeyTable.C`, `KeyTableItem.C`.
  `Include/frame:325,337,347`. 2026-08-03.
- **asOf**: 2026-08-03

---

## THE kant QUESTION

### CLAIM SUP-22 — the kant corpus and the support domain have **zero** textual overlap

- **text**: `Groups/docs/kantCorpus.md` (1,527 lines, 37 claims) mentions **not one**
  support class. Word-greps for `Buffer`, `Stak`, `StringRoutines`, `PLGset`, `CharSet`,
  `DoubleLink`, `BaseHash`, `Tape`, `HashList`, `SearchTree` each return **0**.

  This is a genuinely useful negative. It means **there is no hidden support-domain
  knowledge sitting misfiled in the kant corpus** — no claims that are secretly about
  Buffer or Stak and belong here. Whatever the migration question turns out to be, it is
  *not* "move these claims across", because there are none.

  Combined with SUP-14 — the incant-visible surface is exactly three classes — the two
  domains meet at a **very narrow, explicitly declared seam** (`groups.ext` lines 6, 9,
  12), not at a diffuse boundary that needs adjudicating.
- **confidence**: `inferred`
- **provenance**: `for s in Buffer Stak StringRoutines PLGset CharSet DoubleLink BaseHash
  Tape HashList SearchTree; do grep -cw "$s" Groups/docs/kantCorpus.md; done` → 0 for all
  ten. `grep -nw -e Buffer -e Stak -e StringRoutines Groups/docs/kantCorpus.md` → no
  output. `wc -l` → 1527. 2026-08-03.
- **asOf**: 2026-08-03

### OPEN SUP-23 — should any of kant migrate into the support domain?

*(An OPEN, not a CLAIM, because the entry's most useful sentence describes work nobody
has done. The charter says the recon **informs** this question and does not decide it —
so this minion has deliberately not decided it.)*

- **the question**: Should any part of kant move into the support domain?

- **what IS established** (this round, with provenance above):
  - The seam is **three names**: `PLGset`, `Buffer`, `Stak` in `groups.ext` (SUP-14).
    Everything else in support is invisible to incant.
  - There is **nothing to relocate on the knowledge side** — zero support-class mentions
    across 1,527 lines of kant corpus (SUP-22).
  - The support domain has **spare capacity in exactly the area kant would want**:
    Buffer's mark vocabulary is built but partly uncalled (`insertAtMark`, `shiftAtMark`),
    and `Stak::popOff` likewise — and the 2026-05-28 recon nominated **precisely those**
    for incant wrapping (SUP-11, SUP-12). The support side is already leaning toward kant.
  - Support is **shared by three projects** (`support/CLAUDE.md`), so anything migrated in
    becomes a dependency of PLG and TAWK too, not just Incant. Migration is not free.

- **what is NOT established**:
  - **What "any of kant" refers to.** Corpus claims? `.rtn` runtime glue? incant source
    under `Groups/incant/`? The three readings have different answers and this minion
    cannot tell which is meant.
  - Anything from the kant side. This minion measured the **support half only** and has
    no provenance for kant's internal structure, its dependencies, or what it would cost
    kant to give something up.

- **what it blocks**: nothing today. TASK 2 (Buffer compress + registry) and TASK 3
  (Display) both proceed without an answer. It becomes live if TASK 2's registry grows an
  incant-facing surface, because that is the moment the three-name seam widens.

- **what it would cost to settle**: one pass by someone holding **both** corpora, plus a
  ruling from Tony on which reading of "kant" is meant. Cheap — but it cannot be done from
  inside the support sandbox, which is why it is parked rather than answered.

- **the honest bottom line**: on the support-side evidence alone, **there is no measured
  case for migration and no measured obstacle to it.** A recommendation drawn from one
  half would be an opinion wearing a census's clothes. Flagged to Tony
  (`ipc/support-to-clod.md` SEQ 1, Q4).

- **asOf**: 2026-08-03

---

## OPEN ITEMS

### OPEN SUP-24 — the caller-count scope is not ruled

- **question**: Is NARROW, WIDE or TRACKED authoritative for this corpus?
- **established**: the three differ by 2–3x; `Bot` flips from 0 to 11 between NARROW and
  WIDE; `BaseEntry` flips from 14 to 0 between WIDE and TRACKED (SUP-1, SUP-5, SUP-9).
- **not established**: which the charter intends. The brief scoped *reading* to
  support+Groups; the census question implies the wider tree.
- **blocks**: the candidate list's floor. Everything below is stated at TRACKED and would
  need re-reading (not re-measuring — both columns are recorded) under another ruling.
- **cost**: one sentence from Tony. Parked as SEQ 1 Q1.

### OPEN SUP-25 — per-method attribution needs a tool this round did not have

- **question**: Which class does a given `add`/`get`/`next`/`set` call site belong to?
- **established**: grep cannot tell (SUP-3); the zeros are nonetheless sound.
- **not established**: real per-method caller counts for the ~60 generically-named entry
  points across `DoubleLinkList`, `HashList`, `BaseHash`, `SearchTree`, `CharSet`, `PLGset`.
- **blocks**: any *fine-grained* dead-method list. It does **not** block the file-level and
  distinctive-method findings above, which is most of the census's value.
- **cost**: a type-aware index (clang-based, or parsing the generated `.C` for
  `ClassName::method` definitions and matching receivers). Half a day, and it should wait
  until someone wants method-level pruning — which nobody has asked for.

### OPEN SUP-26 — `Include/TokTests/` is gitignored, on disk, and the sole consumer of live code

- **question**: is `Include/TokTests/` live scratch or abandoned?
- **established**: it is gitignored (`.gitignore`: `Include/TokTests/`), present on disk,
  and contains the **only** references to `BaseEntry` anywhere (SUP-9) plus copies of
  `FormatC.C`, `Tawk.C`, `SearchTree` consumers.
- **not established**: whether it is maintained. Its answer decides `BaseEntry`'s status.
- **blocks**: `BaseEntry`'s place on the candidate list, and nothing else.
- **cost**: one ruling from Tony, same shape as the `BeforeRefactor/` question (SUP-17).

### OPEN SUP-27 — no querier exists for this corpus

- **question**: what reads this file?
- **established**: the charter calls for "a sibling corpus with its own querier". The
  corpus exists; **the querier does not.** `jigcorpus`'s analogue is `jiquery`, a harness.
- **not established**: nothing — this is simply undone work, named so nobody assumes
  otherwise on the strength of the word "sibling".
- **blocks**: nothing in round 1.
- **cost**: small, but a harness carries the H1–H5 obligations (echo its inputs, assert
  its own completeness, wall-clock cap), so it should be built deliberately rather than
  bolted on. Parked as SEQ 1 Q3.

---

## DEAD / PARK-ASIDE **CANDIDATE** LIST

> ⚠ **CANDIDATES ONLY. PARKING IS TONY'S SIGNATURE, NOT THIS MINION'S.**
> *"Unused"* and *"safe to remove"* are different claims, and only the first one was
> measured. Nothing here has been removed, edited, or parked. **No source edits were made
> this round** — the charter forbids them during recon, and the arc-that-was-deleted is
> why.
>
> Ordered by strength of evidence. The right-hand column is the reason **not** to act
> immediately, and it is the more important column.

| # | candidate | evidence | ⚠ reason to pause |
|---|---|---|---|
| 1 | **`SimpleList.twk`** (84 lines) | zero refs at all 3 scopes; no `.C`/`.h`; in **no** build phase; declared in **no** manifest (SUP-6) | Four independent confirmations — the cleanest case in the census. Still: it is 84 lines costing nothing, and it has an uninitialised-return defect that makes it unsafe to *revive* rather than unsafe to keep. |
| 2 | **`URLservice.{twk,mm,h}`** (68 lines) | zero refs at all 3 scopes; not compiled; references an undeclared `file` (SUP-7) | **The only networking code in support**, and TASK 2's registry has "a future web channel behind it". Park-aside, not delete. |
| 3 | **`Stack.{C,h}`** (119 lines) | zero real callers — all 5 hits are the English word in comments; `getTop`/`popFront` zero (SUP-8) | **It is compiled**, so it costs build time silently. But it is one letter from the very-live `Stak`, so any action must be name-checked with extreme care. |
| 4 | **hash cluster** — `HashList`, `HashLink`, `Hasher` (702 `.twk` + 434 `.rtn` lines) | closed triangle; no external consumer (SUP-13) | **Cluster-level only** — the three are mutually dependent, so the zero applies to the set, not the members. Largest single reclaim in the census, and the one most likely to be wrong for a reason not visible to grep. |
| 5 | **8 `StringRoutines` externs** — `alignRIGHT`, `convertNewLine`, `headToChar`, `headToSet`, `stringIsNumeric`, `toStringFromFloat`, `toStringFromLong`, `urlDecode` | zero refs; distinctive names, so airtight (SUP-10) | These are **completed-symmetry** functions (`alignLEFT` is used 14x, `toStringFromInt` 62x). Keeping a symmetric API is a legitimate design choice, and they are ~10 lines each. |
| 6 | **`PrintBuffer` / `StringBuffer` external decls** (`Include/frame:380,476`) | declared; exist nowhere in either tree (SUP-16) | The **only** entry here that is pure documentation debt — nothing to break, since there is nothing to break. But `Include/frame` is a bear-trap #16 hand-sync file, so even this edit is not free. |
| 7 | **`Tape::addSegment`**, **`BaseHash::listKeys`/`removeFromHash`** | zero / near-zero, distinctive names (SUP-12) | Internal helpers of very live classes. Lowest value, highest chance the next feature wants them. |
| — | ~~`Buffer::insertAtMark`, `Buffer::shiftAtMark`, `Stak::popOff`~~ | zero callers | ⚠ **EXPLICITLY NOT CANDIDATES.** All three were nominated by the 2026-05-28 recon as top picks for incant wrapping, and `insertAtMark` is a recorded wish of Tony's (`IncantForms/Notions/flags:16`). **Built ahead of their callers, not abandoned.** Listed here so a later reader does not "discover" them. |
| — | `BaseEntry.{C,h}` | 0 TRACKED, 14 WIDE | ⚠ **NOT a candidate until OPEN SUP-26 is ruled.** Its only consumers are in gitignored `Include/TokTests/`. The answer is entirely a question about that directory. |

**The single most important row is the one with a strikethrough.** Three methods with
zero callers are the *opposite* of dead — they are the next task's raw material. In a
tree that is about to grow, a caller count cannot distinguish *"nobody uses this"* from
*"nobody uses this yet"*, and only intent separates them. That is the whole argument for
rule 3, and this census produced a live example of it rather than a hypothetical.

---

# ROUND 2 — TASK 2 PART A: BUFFER COMPRESS / DECOMPRESS

*The first round that changed source. Everything below rests on an observed run of
`bufferPop/bufferPop.sh`, not on a grep, which is why it carries this corpus' first
`verified` claims about behaviour.*

### CLAIM SUP-28 — `compress` / `decompress` are a self-inverse pair on Buffer, POP'd in isolation

- **text**: `Buffer::compress()` and `Buffer::decompress()` land as **two new methods and
  nothing else** — no new ivar, no bitfield shift, so the `groups.ext` + `tokall` toll the
  charter warns about is **not** owed. The `.h` diff is exactly two lines:
  `int compress();` and `int decompress();`. Buffer's field list is byte-identical before
  and after, which is what makes the tokenizer's blast path (`shorten` → `testContainer`)
  untouched by construction rather than by inspection.

  The pair round-trips **byte-identically** on all eight fixtures, including:

  | fixture | bytes | mode | compressed | note |
  |---|---|---|---|---|
  | `empty` | 0 | s | 10 | the charter's empty case → `BZ1:s:0:0:` |
  | `one` | 1 | s | 12 | one-byte edge |
  | `hello` | 5 | s | 17 | header dominates |
  | `edge` | 19 | z | 17 | one past the 18-byte max match |
  | `bytes` | 256 | s | 356 | **all 256 byte values, so the zero byte head on** |
  | `text` | 2 048 | z | 398 | 19% |
  | `random` | 65 536 | s | 87 400 | LCG stream, incompressible |
  | `large` | 262 144 | z | 46 005 | the charter's large case, 17% |

  ⚠ **The `bytes` row is the load-bearing one and it is not decoration.** `length()` is
  pointer arithmetic, so a Buffer *can* hold a zero byte, but no Buffer accessor can put
  one there (`appendChar` rejects `c == 0`) — and `getFile` (`Groups/Commands.rtn:236`)
  reads with `read()` and sets `current = start + length`, so **a Buffer loaded from a file
  can carry any byte at all**. A pair that round-tripped only NUL-free content would pass
  every string-shaped fixture and lose data on the first binary file the registry archives.
  The fixture writes the bytes through direct field assignment for exactly that reason: the
  byte-exactness claim is worthless if the fixture cannot express a byte the API cannot.

  Also asserted, by value: `decompress` on non-BZ1 content **refuses and changes nothing**
  (returns 0, content intact) rather than guessing; both halves clear the mark (it points
  into content that no longer exists); and the pair **composes** — compress twice,
  decompress twice, original bytes back.
- **confidence**: `verified` — 66 checks, exit 0, output captured.
- **provenance**: `sh ~/data/support/bufferPop/bufferPop.sh` → `bufferPOP PASSED -- 66
  checks, 0 failures`, exit status taken directly from the harness (not through a pipe).
  `.h` diff: `diff Buffer.h.before Frame/Buffer.h` → `31a32,33 > int compress(); > int
  decompress();` and nothing else. 2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-29 — the encoded form needs no escaping in any envelope, and that is the property TASK 2 PART B rests on

- **text**: The wire form is

  ```
  BZ1:<mode>:<rawLen>:<payLen>:<payload>
      mode      z = LZSS packed (4096 window, 3..18 match), s = stored verbatim
      rawLen    byte count of the ORIGINAL content
      payLen    byte count of the payload BEFORE armour
      payload   URL-safe base64, alphabet A-Z a-z 0-9 - _ , unpadded
  ```

  so **the entire compressed form draws only from `A-Z a-z 0-9 : - _`** — no NUL, no
  newline, no space, no quote, no backslash, no `/`, no `+`, no `=`. The POP asserts this
  per fixture (`<tag>.alphabet = 1`) rather than assuming it.

  ⚠ **This is a decision, not a happy accident, and the reason is Buffer itself.**
  `string()`, `toString()`, `tail()` and `appendString` all assume no embedded zero byte.
  A raw-binary compressed payload would leave the object in a state where **its own
  accessors lie about it** — the class would still compile, still link, and quietly
  truncate at the first NUL. Armouring costs 4/3 on the payload and buys a compressed
  Buffer that is still a well-formed Buffer.

  **The consequence for PART B**: a compressed buffer is a **single unquoted word**. Every
  format question that would otherwise have to be answered — how to quote, how to escape a
  newline inside a file's contents, what to do about a `"` in the source being archived —
  **does not arise**. That is why the format proposal (channel SEQ 2) is short.

  ⚠ **The honest cost, stated as a ceiling and not hidden**: stored mode still armours, so
  **incompressible input EXPANDS** — `random` 65 536 → 87 400 (133%), `bytes` 256 → 356
  (139%), `hello` 5 → 17 (340%). The guarantee is *"never worse than 4/3 plus a ~12-byte
  header"*, not *"smaller"*. Text is where it pays: 17–19%. The `mode` character exists so
  a third mode (escape-only, for incompressible **text**, ~1.02x) can be added later
  without a format break; it is not built.
- **confidence**: `verified`
- **provenance**: the `<tag>.alphabet` and `<tag>.mode` rows of the run above, plus the
  printed `<tag>.pct` table. `Frame/Buffer.twk` `compress`/`decompress` read directly.
  2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-30 — ⚠ tok MANGLES AN UNINITIALISED LOCAL ARRAY, and the result does not compile

- **text**: In a `.twk` method body, `char dec[256];` — a local array with **no
  initialiser** — is generated as

  ```c
  char 	dec[] = 0;
  ```

  which is not valid C++ and fails at the compile step, not at `tok` (which exits **0**).
  An array **with** an initialiser is fine, except that **tok also drops the declared
  size**: `char tbl[65] = "ABC…"` becomes `char tbl[] = "ABC…"`. Harmless for a string
  initialiser, and would not be harmless for a partially-initialised numeric table.

  Same family as bear-trap #24 (tok source is not incant source and the failure surfaces
  elsewhere), except this one surfaces in the *compiler* rather than in a wiped extern
  block. It is cheap to avoid once known: `Frame/StringRoutines.twk:535`'s `urlDecode`
  writes all 256 entries out longhand, which is the existing workaround in the tree, and
  the alternative taken here was to **drop the table entirely** and decode base64
  arithmetically (`c - 'A'`, `c - 'a' + 26`, …), which needs no array at all.

  ⚠ **It was found by a throwaway probe, not by the failing build**, and that is the
  transferable part. A 40-line `Probe.twk` exercising every construct the real method was
  going to need — `int *` locals, `break`, `continue`, `1 << n` inside an `if`, an
  initialised char array, an uninitialised one — cost two minutes and turned what would
  have been an opaque compile error inside a 300-line method into one obvious line.
  **Probe the generator before writing the code, not after.**
- **confidence**: `verified` — observed in a run of `tok Probe.twk`, output read.
- **provenance**: probe file with `char tbl[65] = "…"; char dec[256];` → `tok Probe.twk`
  exit 0, generated `Probe.C` containing `char 	tbl[] = "…"` and `char 	dec[] = 0;`.
  2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-31 — the Incant fleet is unmoved, and the claim is non-vacuous because the binary was REBUILT

- **text**: `jitLadder/ladder.sh` → **83 checks, exit 0**. `genLadder/pop.sh` → **32 green,
  exit 1** (the two documented deliberately-unpinned reds, `census.target` and `oneTest
  baseline`). Diffing the before and after logs, **the only line that differs in either is
  RULE H1's own binary echo** — every check row is byte-identical.

  ⚠ **The rebuild is the whole point of the claim.** `~/bin/incant` is a prebuilt binary;
  the Groups target compiles `Frame/Buffer.C` (6 `Buffer.C in Sources` entries in
  `TOK.xcodeproj`). Running the fleet against the **old** binary would have produced the
  same two green results while proving *nothing at all* about a Buffer change — a perfect
  RULE H1 failure, and one that reports as success. So incant was rebuilt first
  (`xcodebuild -project TOK.xcodeproj -scheme Groups -configuration Debug`, **BUILD
  SUCCEEDED**, 0 errors), the new symbols confirmed present with `nm -C` (`Buffer::compress()`,
  `Buffer::decompress()`), and *then* the fleet was run.

  **A fleet check that cannot fail is not a fleet check.** The binary moved
  (1 218 816 → 1 219 088 bytes, sha `fda3fc0…` → `fbbf85b…`) and every row held.
- **confidence**: `verified`
- **provenance**: `xcodebuild … build` exit 0, `grep -c 'error:'` → 0;
  `nm -C "$(readlink ~/bin/incant)" | grep 'Buffer::compress\|Buffer::decompress'` → both `T`;
  `sh jitLadder/ladder.sh` exit 0; `sh genLadder/pop.sh` exit 1 with `grep -c '^  ok'` → 32;
  `diff` of the before/after logs of both → one line each, the H1 binary echo.
  Groups working tree byte-identical before and after (`git status --porcelain` diff empty).
  2026-08-03.
- **asOf**: 2026-08-03

### CLAIM SUP-32 — `bufferPop` is a harness and carries H1–H5, including a demonstrated red

- **text**: `~/data/support/bufferPop/` holds `bufferPop.C` (measures) and `bufferPop.sh`
  (asserts). The split is deliberate: the driver prints every quantity unconditionally as a
  `VAL name = value` line and asserts nothing, so **no check can pass by a line going
  missing** (H4). The harness echoes its sources and its binary (H1) and additionally
  **retoks `Buffer.twk` into a scratch directory and diffs**, so a `Frame/Buffer.C` that
  has drifted from its `.twk` is *reported* rather than silently measured. It caps every
  run at `POPCAP` (default 90s) with sleep-and-kill, mapping 137 → 124 and reporting a
  timeout **by name, never as a diff** (H5). It checks the driver's sentinel **first and by
  name** before reading any other line (H2). Ratios are asserted as **bounds**, never as
  byte counts, because an exact count would go red every time the packer is tuned and say
  nothing about whether the pair is still an inverse (H3).

  ⚠ **The instrument was proven able to go RED, in three ways, before it was trusted.**
  A harness that has only ever been green is not evidence:
  1. sentinel removed → `FAIL SENTINEL ABSENT`, nothing below it read, exit 1;
  2. byte-exactness broken → 8 `identical = 0` rows named individually, exit 1;
  3. driver made to hang with `POPCAP=3` → `FAIL TIMEOUT`, reported by name, exit 1.

  The driver was restored byte-identically after each (sha `47aae48…` before and after).
  ⚠ And one of those three runs re-taught the standing rule the hard way: the first
  negative was read through `| tail -8`, which reported `exit=0` — **the exit status of
  `tail`**. Every subsequent status was taken directly.
- **confidence**: `verified`
- **provenance**: the three negative runs above, each with its harness exit status taken
  directly; `shasum bufferPop.C` before and after → identical; final green run
  `bufferPOP PASSED -- 66 checks, 0 failures`, exit 0. 2026-08-03.
- **asOf**: 2026-08-03

### OPEN SUP-33 — the registry wire format is proposed and NOT ruled; PART B did not start

- **the question**: what format does the registry print action render, such that it reads
  back in?
- **established**: the charter requires the ruling *before* implementation, so nothing was
  built. The proposal is in `Groups/ipc/support-to-clod.md` **SEQ 2**. Two premises for it
  were checked and both hold: `getFile` (`Groups/Commands.rtn:236`) is a **pure byte read**
  — `pushInput` was removed from it (`Groups/TODO.md:591`), so it no longer parses what it
  loads — and a compressed buffer is a single unquoted word (SUP-29), which removes the
  quoting and escaping questions entirely.
- **not established**: the format itself, and the shape of the current incant print path
  for a buffer field (deliberately not reconnoitred past the two premises above — that is
  PART B work and would be running ahead of the ruling).
- **blocks**: all of TASK 2 PART B, which is the load-bearing round-trip POP.
- **cost**: one ruling from Tony.

### OPEN SUP-34 — `Include/frame`'s `external Buffer` mirror does not declare the new pair

- **the question**: when do `compress`/`decompress` get added to the TAWK-side mirror?
- **established**: `Include/frame`'s `external Buffer` block (SUP-14, SUP-15) does **not**
  list them. Nothing needs them yet: PART A is POP'd in isolation against `Buffer.C`
  directly, and the incant build succeeded without them, because the `.h` is generated from
  the `.twk` class body and the mirror only matters for **other** files calling the method.
- **not established**: nothing. This is deliberately deferred work, named so it is not
  discovered at PART B build time.
- **blocks**: any incant or TAWK source calling `buf.compress()` — i.e. PART B.
- **why it was NOT done now**: `Include/frame` is shared with PLG and TAWK, whose builds
  cannot be tested from here, and it is a bear-trap #16 hand-sync target. The addition is
  purely additive (two method declarations, no ivar, no layout change) and is expected to
  be safe — but the round's blast radius was deliberately held to `Frame/Buffer.{twk,C,h}`
  plus a new POP directory, on the same reasoning that left SUP-8's inert `external Stack`
  blocks in place. **It is owed, and it is one edit.**

---

## SOURCES

- `~/data/support` @ **`690dc59ce36f41b86d7f88865f83d58a4b4dd642`** (TASK 0 floor
  snapshot, 2026-08-03, content UNREVIEWED, no edits in commit; tree clean at that SHA)
- `~/data/support/CLAUDE.md`, `.gitignore`, `support.xcodeproj/project.pbxproj`
- `~/data/support/Frame/*.{twk,C,mm,h,rtn}`, `Frame/frameIncludes`, `Frame/devDirectives`
- `~/data/support/Include/{frame,groups.ext,OCframe,globals,tok.ext,PLGrevision,GUIexternals,maps,db.ext,pdf.ext,changes}`
- `InProcess/{Bot,Groups,Parse,TOK,Tokf,wbView}` — read-only, for caller counts
- `Groups/docs/supportMinion.md` (the charter), `Groups/docs/minion-corpus-format.md`
- `Groups/docs/support-extern-recon-2026-05-28.md` — **prior art**, independently
  corroborates SUP-6 (SimpleList "internal"), SUP-13 (hash cluster "internal plumbing"),
  and supplies the intent behind the SUP-11 strikethrough
- `Groups/docs/kantCorpus.md` — for SUP-22 only

**Round 2 additionally:**

- `~/data/support/Frame/Buffer.{twk,C,h}` — **changed** this round (two new methods)
- `~/data/support/bufferPop/{bufferPop.C,bufferPop.sh}` — **new** this round, the harness
- `Groups/Commands.rtn:236` (`getFile`) and `Groups/TODO.md:591` — the two PART B premises
- `Groups/jitLadder/ladder.sh`, `Groups/genLadder/pop.sh` — run, not modified
- `InProcess/TOK/TOK.xcodeproj` — built (Groups scheme, Debug), not modified

## SCOUTS

| id | mission | status | landed |
|---|---|---|---|
| round-1 | TASK 0 (floor snapshot) + TASK 1 (recon census) | absorbed | 2026-08-03 |
| round-2 | TASK 2 **PART A** (Buffer compress/decompress + POP); PART B proposed, not built | absorbed | 2026-08-03 |

*TASK 2 **PART B** (the registry) is **proposed and gated** — the format ruling is owed by
Tony before any of it is implemented (OPEN SUP-33, channel SEQ 2). TASK 3 (Display) has not
been started.*

---

## NOTE FOR THE NEXT ROUND

Five things this round would tell its successor:

1. **Re-run the counts before trusting them.** The commands are all recorded; the tree
   moves. Every number here is `inferred` and dated 2026-08-03.
2. **Check the instrument first.** `grep` is `ugrep`; the shell is `zsh` and does not
   word-split. The census's own first run returned a clean, plausible, entirely false
   zero for every symbol (SUP-2). **Include a known-live control symbol in any sweep.**
3. **Exclude `Frame/BeforeRefactor/`** — it is an exact 21-file name-for-name mirror and
   will silently double any count (SUP-17).
4. **Zero-caller ≠ dead.** Three of the zero-caller methods are the next task's raw
   material. Read the 2026-05-28 recon before nominating anything.
5. **Four questions are parked** in `Groups/ipc/support-to-clod.md` SEQ 1 (scope,
   `BeforeRefactor`, querier, kant). Read Clod's reply in `ipc/clod-to-support.md` before
   assuming this corpus' scope choices were ratified — **they were recommended, not
   approved.** *(Round 2: Q1 ruled — **WIDE is authoritative**, and the read scope is
   formally `~/data/support` plus all of InProcess. Q2 ruled and executed. Q3 answered: no
   querier was owed in round 1; **round 2 built `bufferPop` instead, which is a harness and
   carries H1–H5 in full (SUP-32)** — a corpus querier is still unbuilt. Q4 relayed.)*

**Round 2 adds three, and the first one is the one that would have cost the most:**

6. **A fleet check against a stale binary is not a fleet check.** `~/bin/incant` compiles
   `Frame/Buffer.C`, so *any* change here needs `xcodebuild -project TOK.xcodeproj -scheme
   Groups` **before** the ladders are run, or both ladders go green while proving nothing
   (SUP-31). This is RULE H1 arriving in the support sandbox, where the "binary under test"
   is two repositories away from the file being edited.
7. **Probe the generator before writing the code.** A 40-line throwaway `.twk` exercising
   every construct the real method needed found tok mangling an uninitialised local array
   into `char dec[] = 0;` — two minutes, versus an opaque compile error inside a 300-line
   method (SUP-30).
8. **`Frame/Buffer.C` and `Frame/Buffer.h` reproduce byte-identically from
   `Frame/Buffer.twk`** under a bare `tok Buffer.twk` run from `Frame/` — measured before
   and after the edit, and `bufferPop.sh` now asserts it on every run. So for Buffer, the
   `.twk` really is the source of truth and there is no hidden hand-edit. **Do not assume
   this holds for the other 20 census files; nobody has checked.**
