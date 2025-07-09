# PSI LISP Interpreter

A minimal LISP interpreter written in C for UNIX systems.

## Build and Run

```bash
clang -o psi lisp_interpreter.c
./psi
```

## Usage

```
psi> (+ 1 2 3)
6
psi> (/ 10 2)
5
psi> (= 5 5)
#t
psi> (quit)
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
psi> (+ (* 2 3) (/ 8 2))
10
psi> (= (+ 1 1) (* 1 2))
#t
```

## Error Handling

Common error types:
- `$error{SyntaxError Unbalanced parentheses}`
- `$error{TypeError Arguments to + must be numbers}`
- `$error{ArityError '/' requires exactly 2 arguments}`
- `$error{DivisionByZeroError Division by zero}`

## Limitations

- Unix systems only (untested on other operating systems)
- No user-defined functions
- No variable binding
- 1024 character input limit
- 255 character symbol limit

---

*UNIX-only LISP interpreter*