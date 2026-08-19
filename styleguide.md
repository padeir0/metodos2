# Style-guide

Write C without a style-guide and suddenly you'll see something like:
```c
int a=10000,b,c=2800,d,e,f[2801],g;main(){for(;b-c;)f[b++]=a/5;
for(;d=0,g=c*2;c-=14,printf("%.4d",e+d/a),e=d%a)for(b=c;d+=f[b]*a,
f[b]=d%--g,d/=g--,--b;d*=b);}
```

## Identifier Style

All libs inside `lib` must be a single `.h` file,
named `<XX_nameInCamelCase>.h`.
The name must be in camelCase, the `XX` is a number representing the position of the file
in the class schedule.
Header guards must be `#define M2_<NAMEINCAPS>_H`, so that it
doesn't collide with symbol namespaces.

Underscores `_` are reserved, so that snake_case is not allowed.
Instead we use camelCase for compound names,
and use underscores `_` to separate namespace names and internal
names. Identifier rules are the following:
 - Public names start with `<namespaceName>_`, while internal (private) names
 start with `i_<namespaceName>_`.
 - All public function names must be of the form
`<namespaceName>_<nameCamelCase>`, while private functions
must be of the form `i_<namespaceName>_<nameCamelCase>`.
 - All public macros, constants and enums must be of the form `<namespaceName>_<NAMEINCAPS>`,
while private macros, constants and enums must be of the form `i_<namespaceName>_<NAMEINCAPS>`.
 - All public types must be typedef'd, with names
of the form `<NameUpperCamelCase>`, private types must be
of the form `i_<NameUpperCamelCase>`.
 - All local variables, arguments, struct fields and union fields must be camelCase,
 no need to prefix them with `i_<namespace>_` or `<namespace>_`.
 - All global variables must be prefixed with `g_<namespace>_` and can only be used inside the folders `tests` and `cmd`.

Types often define the namespaces of functions, for example
`matrix` will define the namespace for the functions
`matrix_add`, `matrix_mult`, etc.

## Comment Style

Because a lot of comments in the middle of functions  makes the code harder to read,
i've adopted a policy to keep *large* comments as footnotes. In functions, these footnotes
are just before the closing `}`, while in the middle of the function you'd only have
things like `// NOTE(1)`. If the comment is an one liner that does not exceed 80 columns,
it can be kept in the middle of the function  without resorting to footnotes.
Comments on top of functions serve as docstrings and are encouraged to be descriptive
but short and to the point.

There are, so far, these kinds of notes:
 - `NOTE`: Is a brief explanation of rationale that is not tied to safety.
 - `SAFE`: Explains why some operations are indeed safe.
 - `UNSAFE`: Warns about unsafe situations that are assumed to never happen.
 - `UNTESTED`: Marks functions that have no automated test routine.
 - `TODO`: Things that i *need* to finish.

## Testing

Tests must live inside the `tests` folder. Any file that must be run as part
of an automated test routine must end with `*_test.c`. Files that should
result in a crash due to asserts must end with `*_crash.c`.

Tests should be performed with at least the following flags in `gcc`:

```bash
gcc -Wall -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes -std=c99 -O2
```

But before any commits are made, the code must be tested in `clang`,
with different C standards and optimization levels.
It is sufficient to test for both C99 and C17 (See `tests/testp`),
and both `-O0` and `-O2`. It is best if also tested in C23 and also with `-O3`.

Rationale for this is:
 - The code must be portable between `gcc` and `clang`.
 - The code must behave well _even if_ the compiler abuses undefined behaviours for optimization.
 - The code must *not* use any deprecated features of new C standards.

Tests should not test internal `i_` functions, only public ones.
It is fine for functions that write to `STDOUT` to be untested,
these should be marked as `UNTESTED`.

New things should be tested before the next one, otherwise
a cascade of untested things will collapse this codebase.
Anyway, if something is to be committed without tests,
a `UNTESTED` comment must be added to it.

## Operating System and Architecture

I boldly assume that the code will run inside a POSIX compliant operating system.
I also assume the architecture is either amd64 or arm64.

Most code here should be portable.

## Function declaration

Functions with no arguments should receive an explicit `void` argument instead.
Out-parameters must be the last parameters of a function.
Normal parameters that are passed by pointer and not modified within the function
must be marked `const`.

## Miscelaneous

Other small things:
 - Macros should be used sparingly.
 - Never use global variables inside library code (inside tests, this is fine).
 - Functions that need dynamic allocation should be marked (TODO)
 - Never use `goto`, `continue` or `for` loops.
 - Avoid taking the address of local variables.
 - Never modify procedure arguments, never take their address.
 - All libary functions must be marked with `static inline`.
 - Only depend on standard library.
 - Library configuration through conditional macros should be rather simple.
 - Provide no compatibility with C++.
 - Bad usage and internal errors should crash the program.
 - Recoverable errors should return an error code (or should document the return values).
