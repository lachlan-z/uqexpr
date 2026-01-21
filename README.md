# uqexpr - UQ Expression Evaluator

A command-line expression evaluator program written in C that supports mathematical expressions, variables, and loop variables with range functionality.

## Overview

`uqexpr` is an interactive calculator and expression evaluator that allows you to:
- Evaluate mathematical expressions using the TinyExpr library
- Define and manage variables
- Create loop variables with start, increment, and end values
- Assign expressions to variables
- Print variable values with configurable significant figures
- Read expressions from standard input or from a file

## Features

- **Expression Evaluation**: Evaluate mathematical expressions with support for standard operations and functions
- **Variable Management**: Define variables via command line or during program execution
- **Loop Variables**: Special variables that iterate through a range of values
- **Significant Figures**: Control output precision (2-8 significant figures)
- **Interactive Commands**:
  - `@print` - Display all variables and loop variables
  - `@range <name>,<start>,<increment>,<end>` - Define or update a loop variable
  - `#` - Comment lines (ignored)
- **Assignment Operations**: Assign expression results to variables
- **File Input**: Process expressions from an input file

## Dependencies

- `tinyexpr` library - A small recursive descent parser for math expressions
- Standard C libraries: `stdio.h`, `string.h`, `stdlib.h`, `ctype.h`
- Math library: `-lm`

## Building

The project includes a Makefile for easy compilation:

```bash
make
```

This will compile `uqexpr.c` into an executable named `uqexpr`.

### Makefile Configuration

The Makefile is configured to:
- Use `gcc` with flags: `-Wall -Wextra -pedantic -std=gnu99 -g`
- Include headers from: `/local/courses/csse2310/include`
- Link against: `tinyexpr` and math libraries

## Usage

```
./uqexpr [--forloop string] [--define string] [--significantfigs 2..8] [inputfilename]
```

### Command-Line Options

- `--define <name>=<value>`: Define a variable with a specific value
  - Example: `--define x=5.5`
  - Variable names must be 1-25 alphabetic characters

- `--forloop <name>,<start>,<increment>,<end>`: Define a loop variable
  - Example: `--forloop i,0,1,10`
  - Creates a loop variable named 'i' starting at 0, incrementing by 1, ending at 10

- `--significantfigs <2..8>`: Set the number of significant figures for output (default: 4)
  - Must be between 2 and 8

- `[inputfilename]`: Optional input file containing expressions to evaluate
  - If not provided, reads from standard input

### Interactive Commands

When running interactively (stdin mode), you can:

1. **Evaluate expressions**: Type any mathematical expression
   ```
   2 + 2
   ```

2. **Assign to variables**: Use `=` to create or update variables
   ```
   x = 10
   y = x * 2
   ```

3. **Print variables**: Display all current variables
   ```
   @print
   ```

4. **Define loop variables**: Create or update loop ranges
   ```
   @range i,1,1,10
   ```

5. **Comment lines**: Lines starting with `#` are ignored
   ```
   # This is a comment
   ```

## Examples

### Example 1: Basic Usage

```bash
./uqexpr
```

Interactive session:
```
Welcome to uqexpr.
This program was written by s4808239.
There are no variables.
There are no loop variables.
Please enter your expressions and assignment operations to be evaluated.
2 + 2
4.000
x = 10
@print
Variables:
x = 10.00
There are no loop variables.
```

### Example 2: With Command-Line Variables

```bash
./uqexpr --define x=5 --forloop i,0,1,5 --significantfigs 6
```

This starts the program with:
- Variable `x` set to 5
- Loop variable `i` ranging from 0 to 5 with increment 1
- Output precision of 6 significant figures

### Example 3: From File

```bash
./uqexpr input.txt
```

Where `input.txt` contains:
```
x = 10
y = 20
z = x + y
@print
```

## Exit Codes

The program uses the following exit codes:

- `0`: Successful completion
- `4`: Invalid or unable to open input file
- `5`: Duplicate variables detected
- `11`: Usage error (invalid command-line arguments)
- `12`: Invalid variable specification

## Error Handling

The program validates:
- Variable names (1-25 alphabetic characters only)
- Significant figures range (2-8)
- Loop variable ranges (increment must be non-zero and appropriate for start/end)
- Duplicate variable names
- File accessibility
- Expression syntax

Errors are reported to stderr with descriptive messages.

## Program Flow

1. Parse command-line arguments and initialize variables
2. Display welcome message
3. Print initial variable state
4. Enter main processing loop:
   - Read line from input (file or stdin)
   - Process based on line type:
     - Comments (`#`) - skip
     - `@print` command - display variables
     - `@range` command - update loop variable
     - Expression/assignment - evaluate and optionally assign
5. Continue until EOF
6. Display thank you message and exit

## Development Notes

This program was developed as part of coursework (student ID: s4808239). Some functions were modularized with assistance from Claude 3.5 AI, though all core logic was developed by the author.

### Referenced Functions

The following functions were modularized or assisted by Claude 3.5:
- `command_arg_check()` - Command-line argument processing
- `main_handler()` - Main input processing loop
- `handle_range_command()` - Range command handling
- `print_variables()` - Variable display

The `read_line()` function was provided via EdStem educational platform.

## License

Educational project - refer to course policies for usage restrictions.

## Author

Student ID: s4808239
Course: CSSE2310
