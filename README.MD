# LISP Interpreter

A minimal LISP interpreter written in C for UNIX systems.

## Build and Run

```bash
clang -o lisp_interpretor main.c
./LISP
```

## Usage

```
LISP> (+ 1 2 3)
6
LISP> (/ 10 2)
5
LISP> (= 5 5)
#t
LISP> (quit)
Quitting...
```

## Data Types

- **Numbers**: `42`, `3.14`, `-7`
- **Booleans**: `#t`, `#f`
- **Symbols**: `+`, `hello`, `my-function`
- **Lists**: `(1 2 3)`, `(+ 1 2)`, `()`

## Built-in Functions

### Arithmetic
- `(+ 1 2 3)` → `6`
- `(- 5)` → `-5` (negation)
- `(- 10 3)` → `7`
- `(* 2 3 4)` → `24`
- `(/ 10 2)` → `5`

### Comparison
- `(= 5 5)` → `#t`
- `(= 3 4)` → `#f`

### System
- `(quit)` → exits interpreter

## Examples

```lisp
LISP> (+ (* 2 3) (/ 8 2))
10
LISP> (= (+ 1 1) (* 1 2))
#t
```

## Error Handling

Common error types:
- `$error{SyntaxError Unbalanced parentheses}`
- `$error{TypeError Arguments to + must be numbers}`
- `$error{ArityError '/' requires exactly 2 arguments}`
- `$error{DivisionByZeroError Division by zero}`

# TODO

## Missing Data Types
- **String**: Implement 8-bit character sequences (e.g., `"hello"`, `"I'm Faris"`)
- **Cell**: Add mutable reference cells for read/write operations
- **64-bit Integers**: Replace double-precision floats with 64-bit integer support
- **User-Defined Functions**: Support lambda expressions and closures

## Missing Language Features
- **Variable Binding**: Implement `let` and `define` constructs for variable scoping
- **Special Forms**: Add control flow constructs (`if`, `cond`, etc.)
- **Error Handling**: Expand beyond basic error type with robust error mechanisms
- **String Operations**: Add functions for string manipulation
- **Cell Operations**: Implement read/write operations for reference cells
- **Proper List Operations**: Enhance list manipulation functions

## Implementation Improvements
- **64-bit Integer Arithmetic**: Transition from floating-point to 64-bit integer arithmetic
- **Memory Management**: Develop a better allocation strategy for efficiency
- **Parser Improvements**: Enable parsing of string literals
- **Error Locations**: Add source position tracking for better error reporting
- **Environment/Scope**: Implement a robust variable binding system

## Future Enhancements
- **File I/O**: Support loading and saving programs
- **Standard Library**: Expand built-in function set
- **Performance**: Optimize evaluation for speed
- **Unicode Support**: Add support for international characters

## Limitations

- Unix systems only (untested on other operating systems)
- No user-defined functions
- No variable binding
- 1024 character input limit
- 255 character symbol limit
