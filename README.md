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

The status code is `0` for success, or non-zero if an error occurs. Upon successful completion, two files are generated:

| File name | Purpose |
| --------- | :-----: |
| VERSION.o | Object file to pass to the linker.
| VERSION | Temporary binary file *(can delete)*.

### Run-time

`version.h` defines `get_version_resource`, which is used to access the version information compiled in:

```c
static inline void get_version_resource(version_resource* restrict out)
```

The `version_resource` struct is defined as follows:

```c
typedef struct
{
    uint16_t major;  /* major version */
    uint16_t minor;  /* minor version */
    uint16_t build;  /* build number */
    char notes[256]; /* notes */
} version_resource;
```

#### Example C program

```c
#include <stdio.h>
#include <stdlib.h>
#include "version.h"

int main (int argc, char** argv)
{
    version_resource res;
    get_version_resource(&res);
    printf("major: %u, minor: %u, build: %u, notes: '%s'\n",
        res.major, res.minor, res.build, res.notes);
    return EXIT_SUCCESS;
}
```
- Build with: `gcc -o example example.c VERSION.o -I. -std=c11`
- Outputs: `major: 1, minor: 2, build: 123, notes: '(f00)'`

### Cross-compiling

If you are compiling for an architecture different than your build machine, set the environment variable `LD` to the appropriate linker for that platform. Example:

`LD=arm-linux-gnueabihf-ld mkverobj <args>`
