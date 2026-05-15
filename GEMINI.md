# Gemini Project Context: fastdict

A specialized, template-based dictionary implementation in C that uses a Python-based code generator to create type-specific dictionary variants.

## Project Overview

- **Core Technology:** C (implementation), Python 3 (code generation), C++ (benchmarking/testing).
- **Architecture:** 
  - **Templates:** `dictdef2.h` and `dictdef2.c` serve as templates for the dictionary implementation.
  - **Generator:** `def2.py` is a script that performs search-and-replace on templates based on a JSON configuration.
  - **Configuration:** `data_structures.json` defines the key/value types and behaviors (e.g., `INT-INT`, `STRING-INT`) for the generated dictionaries.
  - **Memory Layout:** Uses a compact entry array (`entries`) and a separate `index_table` for hash-based lookups. This design supports insertion-order traversal while maintaining efficient lookups.

## Building and Running

The project includes a `build.sh` script to automate code generation and compilation.

### Commands

- **Generate & Build:**
  ```bash
  ./build.sh
  ```
  This script executes:
  1. `python3 def2.py -f data_structures.json -o data_structures` (Generates `data_structures.c` and `data_structures.h`).
  2. `clang++ -o dictperf -O3 -DNDEBUG dictperf.cpp data_structures.c` (Builds performance test).
  3. `clang++ -o unit -O0 -ggdb unit.cpp data_structures.c` (Builds unit tests).

- **Run Tests:**
  ```bash
  ./unit
  ```

- **Run Benchmarks:**
  ```bash
  ./dictperf
  ```

## Development Conventions

### Adding New Dictionary Types
1. Open `data_structures.json`.
2. Add a new object to the `"dictdef2.*"` list with the required replacement keys:
   - `$TT`, `$Tt`, `$tt`: Type names in UPPER, Pascal, and lower case.
   - `$K`: C type for the Key (e.g., `int`, `char *`).
   - `$VV`, `$Vv`, `$vv`: Value names in UPPER, Pascal, and lower case.
   - `$U`: C type for the Value.
   - `$EQUAL_KEYS`: Expression for key equality (e.g., `a == b` or `strcmp(a, b) == 0`).
   - `$FREEKEY` / `$FREEVALUE`: Logic to free keys/values (use `NULL` if not needed).

### Template Syntax
The templates (`dictdef2.h`, `dictdef2.c`) use `$` prefixed variables for code generation:
- `$H_INCLUDES`: Additional headers for the `.h` file.
- `$C_INCLUDES`: Additional headers for the `.c` file.
- `$OPT_KEY_IS_PTR`: Boolean flag for pointer optimization.
- `$OPT_HASH_USE_DEFAULT`: Use the built-in FNV1A hash if true.

### Coding Style
- **C Implementation:** Follows a pseudo-object-oriented pattern in C (`dict_type_function(self, ...)`).
- **Memory Management:** Configurable via `$MALLOC`, `$REALLOC`, and `$FREE` in the JSON config. Defaults to standard library functions.
- **Safety:** Extensive use of `assert()` for internal consistency checks.
