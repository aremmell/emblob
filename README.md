# mkverobj
cli utility and header for link-time embedding of a version resource for use with gcc/g++

## About

**TL;DR:** `mkverobj` is a simple cli utility that generates an object file that can be linked in to an executable *(or library)*, along with a header that defines a version resource data structure and a function to retrieve the compiled version information at runtime.


## Compiling

There's just a simple `Makefile` as of now, which may or may not work for you. Clone and `make`. Creates `build/mkverobj`.

## Usage

The intention is to use `mkverobj` as part of your build process&mdash;before the linking stage, then include the header to access the version information in your code.

### Compile-time

Simply run `mkverobj` with the version information you'd like as arguments: major, minor, build, and notes *(i.e., commit hash)*.

For example, for version *1.2.123 '(f00)'* :

`mkverobj "1" "2" "123" "(f00)"`

The status code is `0` for success, or non-zero if an error occurs. Upon successful completion, an object file named `VERSION.o` is generated. You can then pass it to the linker in your build process.

### Run-time

`version.h` defines `get_version_resource`, which is used to access the version information:

```c
static inline const version_resource* get_version_resource(void)
```

as well as a few other helpers to access individual pieces of information:

```c
static inline uint16_t get_version_major(void)
static inline uint16_t get_version_minor(void)
static inline uint16_t get_version_build(void)
static inline const char* get_version_notes(void)
```

The `version_resource` struct is defined as follows:

```c
typedef struct
{
    uint16_t major;  /** major version */
    uint16_t minor;  /** minor version */
    uint16_t build;  /** build number */
    char notes[MAX_VER_NOTES]; /** notes (e.g. commit hash, date) */
} version_resource;
```

#### Example C program

- Build with: `gcc -Wall -o build/example example.c VERSION.o -I. -std=c11`

```c
#include <stdio.h>
#include <stdlib.h>
#include "version.h"

int main (int argc, char** argv)
{
    const version_resource* res = get_version_resource();
    printf("major: %u, minor: %u, build: %u, notes: '%s'\n",
        res->major, res->minor, res->build, res->notes);
    return EXIT_SUCCESS;
}
```

#### Example C++ program

- Build with: `g++ -Wall -o build/example++ example.cc VERSION.o -I. -std=c++17`

```cpp
#include <iostream>
#include <cstdlib>
#include "version.h"

int main (int argc, char** argv)
{
    auto res = get_version_resource();
    std::cout << "major: " << res->major << ", minor: " << res->minor << ", build: " << res->build
        << ", notes: '" << res->notes << "'" << std::endl;
    return EXIT_SUCCESS;
}
```

### Cross-compiling

If you are compiling for an architecture different than your build machine, set the environment variable `LD` to the appropriate linker for that platform. Example:

`LD=arm-linux-gnueabihf-ld mkverobj <args>`
