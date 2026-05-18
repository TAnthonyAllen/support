# CLAUDE.md — Support Repository

This file orients Claude Code (Clod) when working in this repository.
See `projectBible.md` in the PLG repo for full ecosystem context.

---

## What This Repository Is

Shared support classes used by all three projects in the ecosystem:
- **PLG** (https://github.com/TAnthonyAllen/plg) — parser/generator
- **TAWK** (https://github.com/TAnthonyAllen/tawk) — transpiler (repo pending)
- **Incant** (https://github.com/TAnthonyAllen/incant) — extensible language (repo pending)

These classes are foundational — they don't depend on each other or on the three main projects.
Changes here affect all three projects. Be careful.

---

## Repository Structure

```
support/
├── Frame/          — Core C++ support classes
│   ├── Buffer      — Growable text buffer, malloc-based, writable memory
│   ├── DoubleLinkList / DoubleLink  — Doubly linked list with safe iteration
│   ├── PLGset      — Character set for PLG pattern matching
│   ├── BaseHash    — String-keyed hash table
│   ├── Stak        — Stack (push/pop)
│   ├── Tape        — Fast fixed-size object allocator
│   └── StringRoutines — String utility functions
├── KeyTable/       — Keyword lookup support
│   ├── KeyMap
│   ├── KeyTable
│   └── KeyTableItem
├── Maps/           — Bitmap and mapping support
│   ├── BitMAP      — Bitmap data structure
│   └── Segment     — Segment mapping
├── Include/        — TAWK externals (tawk's equivalent of .h files)
│   ├── frame       — Externals for Frame classes
│   ├── maps        — Externals for Maps classes
│   ├── globals     — Global definitions
│   ├── plg.ext     — PLG-specific externals
│   └── groups.ext  — Incant-specific externals
└── CLAUDE.md
```

**Local path**: `~/data/support/`  
**Symlinks from InProcess**: `Frame`, `Include`, `KeyTable`, `Groups/Maps` in InProcess all point here.

---

## Key Classes

### Buffer
Growable text buffer. All PLG input lives in Buffers.
Key fields: `start`, `current`, `end`, `mark`
Key methods: `appendString()`, `reset()`, `setMark()`, `getMarkedString()`, `toString()`
Aliases: `+=` for append, `-=` for delete

### DoubleLinkList / DoubleLink
Doubly linked list. Used everywhere — rules, alternatives, elements, children.

**CRITICAL — safe iteration pattern:**
```cpp
// SAFE — each caller has its own link pointer, no shared state
for (DoubleLink *link = list->first; link; link = link->next) {
    MyClass *item = (MyClass*)link->value;
}

// UNSAFE in recursive contexts — uses shared entry field, gets clobbered
while (item = list->next()) { ... }
```

The safe `next(DoubleLink*&)` overload also exists:
```cpp
DoubleLink *link = null;
while (item = list->next(link)) { ... }
```

In TAWK source use the for loop pattern:
```
for link = list.first {
    item = link.value;
    link = link.next; }
```

### PLGset
Character set for PLG matching. Supports ranges (`a-z`), negation (`^`), escape sequences.
Key methods: `contains(char)`, `skip(String)` — note `skip()` returns `String` (advanced cursor), not int.
Constructor: `new PLGset("0-9")` or `new PLGset("a-zA-Z")`

### BaseHash
String-keyed hash table storing `void*` values.
```cpp
hash["key"] = (void*)value;    // store
MyClass *v = (MyClass*)hash["key"];  // retrieve
```

### Stak
Simple stack. `push(void*)`, `pop()` returns `void*`.
Used for input diversion stack in PLGparse.

### Tape
Fast fixed-size object allocator. Pre-allocates a block and hands out strips.
`getStrip()` returns next available slot. `free()` returns to pool.
Used by DoubleLinkList for DoubleLink allocation.

---

## File Types

- `.C` / `.h` — C++ source and headers
- `.mm` — Objective-C++ (used by Incant)
- `.twk` — TAWK source (where applicable)
- `frame`, `maps`, `globals` etc. in Include/ — TAWK externals (no extension by convention)

---

## Important Notes

1. **These are shared** — a change to Buffer affects PLG, TAWK, and Incant simultaneously.
2. **PLGset.skip() returns String** — not int. It returns the advanced cursor position. Callers must capture: `cursor = skipSet->skip(cursor)`
3. **DoubleLinkList.next() is not thread safe** — uses shared `entry` field. Use the `for link = list.first` pattern in any recursive context.
4. **Buffer memory is writable** — unlike C string literals. This is intentional and important for PLG's string() method.
5. **Tape is used by DoubleLinkList** — `DoubleLinkList::globalLinkTape` must be initialized before any list operations.

---

## Working Relationship

**Anthony** — architect, final authority.
**Claude (claude.ai chat)** — design and reasoning.
**Clod (Claude Code)** — execution, file changes, GitHub.

Standing permissions: Change any code in source directories without asking.
Trivial repo operations (commits, pushes for routine work) happen at Clod's
discretion; flag non-trivial or uncertain situations before acting. Same
commit-and-push freedom — no verification round-trip on routine work.
