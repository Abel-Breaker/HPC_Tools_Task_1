# Style Guide
This project is configured with a **strict quality-first workflow**. Although it might seem counterproductive and take some time getting used to at first, maintaining such strict code consistency fosters long-term maintainability. Using warnings, analyzers and sanitizers catches many bugs early on and saves you countless hours.

Prioritize **simplicity** over clever or "guru-level" implementations. In most cases, it's better to **write code that is easy for both the compiler and other developers to understand** than to rely on micro-optimizations that add complexity with little practical benefit. Simpler code is easier to maintain, review, test, and evolve.

General philosophy:
- Keep it simple
- *Make it work, make it right, make it fast*
- Follow the Single Responsibility Principle
- Small and focused files and functions

All code must be formatted using the provided `.clang-format` configuration.

Below is a style guide to follow (very similar to [**Linux Kernel Coding Style**](https://www.kernel.org/doc/html/v4.10/process/coding-style.html), with a few variations).


## 1) INDENTATION
Tabs of **8** characters.

## 2) LENGTH OF LINES
The limit on the length of lines is **100 columns**.


## 3) PLACING BRACES
Put the **opening brace last on the line**, and put the **closing brace first** to all non-function statement blocks (if, switch, for, while, do), thusly:
```
if (x is true) {
        we do y
}
```

**EXCEPT for functions**, which have the opening brace at the beginning of the next line, thus:
```
int function(int x)
{
        body of function
}
```


## 4) SPACES
Use a space after (most) keywords. The notable exceptions are sizeof, typeof, alignof, and \_\_attribute\_\_ defined.

When declaring pointer data or a function that returns a pointer type, the preferred use of * is adjacent to the data name or function name and not adjacent to the type name, e.g. `char *name;`

Use one space around (on each side of) most binary and ternary operators, such as any of these: ``` =  +  -  <  >  *  /  %  |  &  ^  <=  >=  ==  !=  ?  : ```
but no space before and after the postfix increment & decrement unary operators: ``` ++  -- ```, and no space around the `.` and `->` structure member operators.

## 5) NAMING
Variable and function names should be **short** but **descriptive**. The writing style is **snake_case** for everything except for MACRO and ENUMERATION_CONSTANTS. Also use **PascalCase** for struct or enum types.

Function names should start with the name of their header, module, or main concept whenever practical. This keeps related functions easy to identify and discover.

**Create/destroy:** Use `create` and `destroy` when a function allocates and returns a structure, or destroys it and releases its memory.
   - `create`: Allocate and return the structure.
   - `destroy`: Destroy the structure and release its memory.

**Init/cleanup:** Use `init` and `cleanup` for an existing structure.
   - `init`: Prepare an existing structure for use.
   - `cleanup`: Release its resources and leave it in a clean state.

**Startup/shutdown:** Use `startup` for functions that must be called once before using a system or component, and `shutdown` for functions that must be called once when it is no longer needed.

## 6) COMMENTING
NEVER try to explain HOW your code works in a comment: it’s much better to write the code so that the working is obvious, and it’s a waste of time to explain badly written code. Generally, you want your comments to tell WHAT your code does, not HOW.

Also, try to avoid putting comments inside a function body: if the function is so complex that you need to separately comment parts of it, it may indicate that the function needs to be broken down into more than one function.

Use full English words in documentation and comments. Write do not, not don't.

**One-line** comments must have a space between the comment delimiter and the comment, like this:
```
// This is the preferred style for one-line comments.
```

The preferred style for long (**multi-line**) comments is:
```
/*
 * This is the preferred style for multi-line
 * comments. Please use it consistently.
 *
 * Description: A column of asterisks on the left side,
 * with beginning and ending almost-blank lines.
 */
```
All **public functions must have a doxygen style comment**. It is also recommended to do the same in statics functions. E.g.
```
/**
 * @brief Program example.
 *
 * @param[in] argc Argument Count.
 * @param[in] argv Argument Vector.
 *
 * @note Some notes!
 */
int main(int argc, char **argv)
{
	printf("Hello world!\n");

	return EXIT_SUCCESS;
}
```

It is recommended specify if a function is thread-safe in the `@note`.

## 7) VARIABLES
Try to **add as much information as possible to the variables via their type and context**:
- For example, if you know a int cannot be negative use unsigned int instead.

- Always use `const` if you know the variable won't be modified.

- Use static variables within the .c files.

You must inicializate all structs and pointers when declared, e.g. ```struct data tmp = {}; char *name = nullptr;```.



## 8) FUNCTIONS
They should be **short** and perform a **single task**.

Excessively long functions or function names are a bad sign; it may indicate that the function needs to be broken down into more than one function.

Functions without arguments should specified void, e.g. ```int foo(void)```.

### Centralized exiting of functions
Yes, you can use ```goto``` statement when:
- unconditional statements are easier to understand and follow
- nesting is reduced
- errors by not updating individual exit points when making modifications are prevented
- saves the compiler work to optimize redundant code away ;)

### Order
In a .c file first write all private/static functions then all public functions.


## 9) FILES
Always separate the .h file and the .c file(s).

The order in files are:

1. Descriptive comment and copyright comment (if applicable)
2. The prototype/interface header for this implementation (if applicable)
3. Project includes (always using double quotes)
4. Standard includes
5. Static constants and types (if applicable)
6. Static variables (if applicable)
7. Static functions (if applicable)
8. Global functions

Include the same dependencies in a `.c` file that its corresponding `.h` requires. This makes dependencies explicit and simplifies project modifications.

Remove unnecessary includes and keep the include list minimal and organized as commented below and in alphabetical order. 

Always use relative paths.

Keep the same order across related declarations and implementations. For example, if an enum is defined as `a, b`, handle its cases in the same order in the corresponding `switch`.

Document the public API in the .h file. The .c file may contain more detailed comments explaining internal implementation details for developers.

Use a maximum of 2 consecutive blank lines to separate logical blocks within a file(e.g. function declarations from includes or distinct code blocks within the same function). Use one blank line for all other cases.

## 10) TYPEDEFS
**Try to minimize the use of typedefs**, as they can obscure the code. In fact, they should only be used to obscure structures (what you should avoid) or make compile-time types. It is better to read `struct device *dev;` than `device_t *dev;`.

For compile-time types use suffix *_t*.

## 11) MACROS
Try to avoid macro functions and try to reduce the number of global macros in the program.

## 12) SWITCH
Always add a default case, if defaults do nothing put literally:
```
default:
        // Nothing to do
        break;
```
This type of comment makes it clear that it is 'ok' if not all cases are covered, as opposed to other switch statements where this would be 'not ok'.


## 13) MEMORY MANAGEMENT
### Allocating
The preferred form for passing a size of a struct is with the number of elements n first (if necessary) followed by sizeof of the variable or struct NOT THE TYPE directly, e.g. 
```
struct example *p = malloc(n * sizeof(*p));
```

NEVER cast a malloc, alloc, etc.

### Freeing
Always set freed pointers to nullptr.


## 14) CONDITIONAL COMPILATION
Wherever possible, don’t use preprocessor conditionals (#if, #ifdef) in .c files; doing so makes code harder to read and logic harder to follow. Instead, use such conditionals in a header file defining functions for use in those .c files.

Prefer to compile out entire functions, rather than portions of functions or portions of expressions.

## 15) ERROR MANAGEMENT
Functions can handle errors internally; there is no need to pass everything up the call stack. Handle errors as deemed necessary: ​​propagate the error or return an error indicator if the situation requires it; otherwise, do not. It can return a bolean or an error value. 

Always use the [error_handler.h](src/utils/errors/error_handler.h).

## 16) OTHERS
Use C23 keyword `nullptr` instead of `NULL`.

Use C23 keywords `bool`, `false` and `true` instead of `"stdbool.h"` directives.

Use `#pragma once` directive on .h files.

Use ++tmp instead of tmp++ unless post-increment semantics are required.

If code from another library or project is copied into the project, include its license in [docs/THIRD_PARTY_LICENSES](docs/THIRD_PARTY_LICENSES.).