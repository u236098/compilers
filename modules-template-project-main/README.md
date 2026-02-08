# Compilers Practice 2 — Lexical Analysis (Scanner)

## Overview

This project implements a lexical analyzer (scanner) for a subset of C.
It reads a `.c` input file and produces an in-memory token list, then writes
a `.cscn` output file containing the recognized tokens.

The scanner is built as a modular C project with CMake. Each module is a
separate static library that can be developed, compiled, and tested in
isolation.

---

## Features

- **Character-by-character scanning** using a DFA transition matrix (no string library for keyword recognition in the input stream).
- **Token categories**: `CAT_NUMBER`, `CAT_IDENTIFIER`, `CAT_KEYWORD`, `CAT_LITERAL`, `CAT_OPERATOR`, `CAT_SPECIALCHAR`, `CAT_NONRECOGNIZED`.
- **Keywords**: `if`, `else`, `while`, `return`, `int`, `char`, `void`.
- **Operators**: `=`, `>`, `+`, `*`.
- **Special characters**: `(`, `)`, `;`, `{`, `}`, `[`, `]`, `,`.
- **Numbers**: integer sequences `[0-9]+` (leading zeros allowed).
- **Identifiers**: `[A-Za-z][A-Za-z0-9]*` (not a keyword).
- **Literals**: `"..."` (double-quoted strings).
- **Non-recognized**: consecutive invalid characters grouped into a single token with one error message.
- **Unterminated literal** detection and error reporting.
- **Output format**: RELEASE (tokens per source line) or DEBUG (with line numbers and separator lines), controlled by `OUTFORMAT` preprocessor option.
- **Operation counting** with `COUNTCONFIG`, `COUNTCOMP`, `COUNTIO`, `COUNTGEN` preprocessor flags.
- **Centralized error handling** with error IDs, phase (step) identifiers, and context messages.
- **Logger** routing (stdout or file) controlled by `DEBUG_FLAG`.
- **Future parser hook**: in-memory token list ready for parser consumption.

---

## Project Structure

```
modules-template-project-main/
├── CMakeLists.txt          # Top-level CMake config (builds scanner + tests)
├── src/
│   ├── main.c / main.h     # Scanner driver (CLI entry point)
│   ├── utils_files.c/.h    # Utility library (log filename generation)
│   ├── lang_spec/           # Language specification constants & helpers
│   ├── char_stream/         # Input cursor (peek/get, line/col tracking)
│   ├── token/               # Token data object
│   ├── token_list/          # Ordered token list (dynamic array)
│   ├── automata/            # Scanner engine (DFA transition matrix)
│   ├── out_writer/          # .cscn output file writer (RELEASE/DEBUG)
│   ├── error_mod/           # Error catalog & reporter
│   ├── logger/              # Debug message router (stdout/file)
│   ├── counter/             # Operation counting system
│   ├── module_args/         # (template) CLI argument processing
│   └── module_2/            # (template) Fibonacci example
├── tests/
│   ├── test_scanner.c/.h    # Scanner module tests
│   ├── test_module_args.c/.h
│   ├── test_module_2.c/.h
│   ├── test_modules.h
│   └── example.c            # Sample input file for scanner
└── build/                   # CMake build output (git-ignored)
```

---

## Module Descriptions

| Module       | Responsibility                                    |
|-------------|---------------------------------------------------|
| `lang_spec`  | Language constants, keyword table, char classifiers |
| `char_stream`| File input cursor with peek/get and line/col      |
| `token`      | Token data structure (lexeme, category, line, col) |
| `token_list` | Growable dynamic array of tokens                  |
| `automata`   | DFA-based scanner engine with transition matrix   |
| `out_writer` | Writes .cscn file in RELEASE or DEBUG format      |
| `error_mod`  | Error catalog with IDs, steps, and message templates|
| `logger`     | Routes messages to stdout or file via DEBUG_FLAG  |
| `counter`    | Operation counting (COUNTCOMP/IO/GEN)             |

---

## Building the Project

### Prerequisites
- CMake ≥ 3.10
- GCC (C11 support)

### Build Commands

```bash
cd modules-template-project-main
mkdir -p build && cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Build with operation counting enabled:

```bash
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-DCOUNTCONFIG"
cmake --build .
```

---

## Running the Scanner

```bash
./build/scanner_main <input.c>
```

**Example:**

```bash
./build/scanner_main tests/example.c
```

This produces `tests/example.cscn` with the token output.

### Sample Output (RELEASE format)

Input (`example.c`):
```c
if(x > 3)
 printf("true");
else
 printf("false");
```

Output (`example.cscn`):
```
<if, CAT_KEYWORD> <(, CAT_SPECIALCHAR> <x, CAT_IDENTIFIER> <>, CAT_OPERATOR> <3, CAT_NUMBER> <), CAT_SPECIALCHAR>
<printf, CAT_IDENTIFIER> <(, CAT_SPECIALCHAR> <"true", CAT_LITERAL> <), CAT_SPECIALCHAR> <;, CAT_SPECIALCHAR>
<else, CAT_KEYWORD>
<printf, CAT_IDENTIFIER> <(, CAT_SPECIALCHAR> <"false", CAT_LITERAL> <), CAT_SPECIALCHAR> <;, CAT_SPECIALCHAR>
```

---

## Running Tests

```bash
cd build
ctest --output-on-failure
```

Or run the scanner test directly:

```bash
./build/tests/test_scanner
```

---

## Configuration Flags

| Flag           | Purpose                                    | Default  |
|---------------|-------------------------------------------|----------|
| `OUTFORMAT`   | Output format: `0`=RELEASE, `1`=DEBUG     | RELEASE  |
| `DEBUG_FLAG`  | Message routing: `0`=stdout, `1`=file     | 0 (OFF)  |
| `COUNTCONFIG` | Enable operation counting macros           | undefined (OFF) |

Set via CMake:
```bash
cmake -DCMAKE_C_FLAGS="-DOUTFORMAT=1 -DDEBUG_FLAG=1 -DCOUNTCONFIG" ..
```

---

## Notes

- The `build/` directory is ignored by git.
- The scanner is designed to be extended: the in-memory token list can be
  passed directly to a future parser module without re-reading the file.
- All language-dependent constants are in `lang_spec/lang_spec.h`.
- No string library functions are used for keyword recognition in the
  input stream (character-by-character comparison only).

