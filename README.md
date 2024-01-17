# emblob

<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com> -->

[![License](https://img.shields.io/github/license/aremmell/emblob?color=%2340b900&cacheSeconds=60)](https://github.com/aremmell/emblob/blob/master/LICENSE)
[![REUSE status](https://api.reuse.software/badge/github.com/aremmell/emblob)](https://api.reuse.software/info/github.com/aremmell/emblob)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=aremmell_emblob&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=aremmell_emblob)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=aremmell_emblob&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=aremmell_emblob)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=aremmell_emblob&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=aremmell_emblob)

A CLI tool to be used as part of a build toolchain which generates linker input object files containing arbitrary files as binary objects (blobs) with accompanying C/C++ header files providing the necessary code to access the blob's data at runtime.

To put it in simple terms, emblob allows you *include as part of your executable* files such as, but not limited to:

- Videos, images, sound effects/music tracks
- Configuration files (*JSON/YAML/TOML/INI/XML, etc.*)
- Version information (*commit hash, time/date, build machine, etc.*)
- Web payloads (*HTML/CSS/JS, etc.*) for web-based GUIs
- Public keys/certificates

In fact, any file can be embedded using emblob. Upon deployment of your application, you can utilize the automatically generated code to gain direct access to the raw data (*on a per-file basis*) of the files that are embedded in your executable (blobs). You can then do whatever you wish with this data: extract to disk, stream over a network connection, display as part of a GUI, and more.

<!-- toc -->

- [Building from source](#build-from-source)
  - [Build products](#build-products)
    - [Generated code](#generated-code)
      - [Functions](#generated-functions)
- [Command-line interface](#cli-interface)
  - [Options](#cli-options)
  - [Examples](#cli-examples)
    - [Bare minimum](#bare-minimum)
    - [Data structures](#data-structures)
  - [Using a specific compiler frontend](#using-specific-compiler)

<!-- tocstop -->

## <a id="build-from-source" /> Building from source

The easiest way to build emblob is by using VS Code and the CMake Tools extension:

1. Load `emblob.code-workspace` and bring up the command palette
2. Select `CMake: Select Configure Preset` and choose `debug` or `release`
3. Select `CMake: Build`

If you would rather just use CMake from the command-line:

1. `cmake -S . -B build`
2. `cmake --build build --target emblob --target cexample --target cxxexample --clean-first`

### <a id="build-products" /> Build products

<!-- The CMake configuration is designed to be multi-purpose; it compiles emblob as an executable in the `build` directory, then it executes emblob with a test input file, `test.bin`. This is a 250-byte binary file with the first 15 bytes set to 0x1 through 0xf and the rest zeroes. emblob generates several files:

- `test.S`: A linker assembly file containing instructions for the linker to embed `test.bin` into `test.o`
- `test.o`: A linker input object file which contains `test.bin` as a binary blob
- `emblob_test.h`: A C/C++ header file containing routines to access binary blob data.

Following this, two example executables are compiled from the corresponding source files in the `example` directory and linked with `test.o`. They are placed in the `build` directory, and are named `cexample` and `cxxexample`.

These programs are bare-bones demonstrations of basic usage of the emblob-generated files. When run, they perform an iteration of the first 15 bytes in the embedded binary blob and print the hexadecimal values to stdout. -->

#### <a id="generated-code" /> Generated code

One of emblob's build products is a C and C++ compatible header file containing generated code that provides quick and easy access to an embedded binary blob's data at runtime. The name of the header is dependent upon the value passed to emblob on the command-line for the `--outfile/-o` option. If you do not specify a value for this option, the basename of the input file is used. The header file's name will always be in the format `emblob_{outfile}.h`.

##### Example filenames

- `emblob -i gorilla.html` &rightarrow; `emblob_gorilla.h`
- `emblob -i gorilla.html -o chimpanzee` &rightarrow; `emblob_chimpanzee.h`

##### <a id="generated-functions" /> Functions

All generated functions have names in the format `emblob_get_{outfile}_{what}` where `{what}` represents the function's specific task or return type. All functions are also declared `static inline`.

1. `uint64_t emblob_get_{outfile}_size()`: Returns the size of the embedded blob, in bytes.
2. `const uint8_t* emblob_get_{outfile}_8()`: Returns a pointer to the embedded blob that may be used to access the blob's data one byte (8-bits) at a time.
3. `const uint16_t* emblob_get_{outfile}_16()`: Returns a pointer to the embedded blob that may be used to access the blob's data two bytes (16-bits) at a time.
4. `const uint32_t* emblob_get_{outfile}_32()`: Returns a pointer to the embedded blob that may be used to access the blob's data four bytes (32-bits) at a time.
5. `const uint64_t* emblob_get_{outfile}_64()`: Returns a pointer to the embedded blob that may be used to access the blob's data eight bytes (64-bits) at a time.
4. `const void* emblob_get_{outfile}_raw()`: Returns a pointer to the embedded blob that may be used to access the blob's data arbitrarily.

# <a id="cli-interface" /> Command-line interface

emblob's command-line interface is quite simple.

## <a id="cli-options" /> Options

| Name | Short name | Description | Default value |
|:-----------|:-----|:------------|:-------------:|
| `--infile` | `-i` | The relative path of the file to embed as a binary blob. | N/A |
| `--outfile` | `-o` | The *basename* of the output files (e.g. 'foo' will result in foo.S, foo.o, and emblob_foo.h). | Basename of the input file |
| `--log-level` | `-l` | Sets the console logging verbosity: [debug, info, warning, error, fatal]. | info |
| `--version` | `-v` | Prints emblob version information. | N/A |
| `--help` | `-h` | Prints emblob usage information. | N/A |

## <a id="cli-examples" /> Examples

### <a id="bare-minimum" /> Bare minimum

At the minimum, emblob requires the input file name. This could be a text file for example, and will use defaults for all other options:

~~~sh
emblob --infile test.bin
~~~

### <a id="data-structures" /> Data structures

One handy way to use emblob is to embed C-style data structures, then simply obtain a type-safe pointer and access members. This is fairly straightforward to accomplish; follow the below steps in order to build your own example program that includes an embedded data structure generated from a binary file and accesses it at runtime.

1. Create a C++ source file, and place the following struct's definition somewhere near the top. Save it as `struct_example.cpp`:

   ~~~cpp
   struct MyStruct
   {
     uint32_t magic = 0U;
     uint16_t secret_id = 0;
     uint8_t text_area[16] {};
   };
   ~~~

2. Using a hex editor (like [this](https://hexed.it/) free online one) and create a new file (*I've done this step already; download [my file](https://rml.dev/pub/struct.bin)* and skip ahead if you'd like). Put the desired value of `magic` in the first 4 bytes, the value of `secret_id` in the next 2 bytes, and finally, place some ASCII characters values after that, ensuring that at least the last character's value is 0x00, and that there are exactly 16 places for ASCII characters to go. When finished, your file should be precisely 22 bytes in size.

   For this example Let's use `0x12345678` for `magic`, `0xABCD` for `secret_id`, and the ASCII characters (*see [this table](https://www.asciitable.com/) for relevant hexadecimal byte values*) `"Hello, world.\0"` for `text_area`.

   > Note: this example does not take into account the [endianness](https://en.wikipedia.org/wiki/Endianness#Byte_addressing) of your system. The file I created is in little-endian format.

3. Save the file as `struct.bin` in the same directory that you've cloned emblob into.

4. Build emblob if you haven't already, then execute `build/emblob --infile struct.bin`. You should now have two files of interest:

- `emblob_struct.h` : Contains the auto-generated code to access the embedded structure's data.
- `struct.o` : The structure is embedded in this file, which will be made part of our sample program in step 7.

5. Place the following additional code in `struct_example.cpp` so that it resembles the following:

   ~~~cpp
   #include <cstdlib>
   #include <cstdio>
   #include "emblob_struct.h"

   struct MyStruct
   {
     uint32_t magic = 0U;
     uint16_t secret_id = 0;
     uint8_t text_area[16] {};
   };

   int main()
   {
     // Obtain a typed pointer to the data structure by accessing it via
     // an emblob auto-generated function, and casting it to the right type.
     const auto typed_ptr = static_cast<MyStruct*>(emblob_get_struct_raw());

     // Print out the values the structure contains.
     printf("Embedded structure: magic = 0x%08X, secret_id = 0x%04X, text_area = '%s'\n",
       typed_ptr->magic, typed_ptr->secret_id, static_cast<const char*>(typed_ptr->text_area));

     return EXIT_SUCCESS;
   }
   ~~~

7. Execute `c++ -c struct_example.cpp && c++ -o build/struct_example struct_example.o struct.o` in your terminal.

You should now have an executable at `build/struct_example` and can execute it in the terminal to view its output. If you see the expected values being printed, then you have successfully embedded and accessed at runtime a C-style data structure based on the contents a binary file!

## <a id="using-specific-compiler" /> Using a specific compiler frontend

When choosing a compiler frontend, emblob will attempt to read the `CC` environment variable. If it is empty, emblob will execute `cc`.

In order to choose a specific compiler frontend, simply set the `CC` environment variable to the name of the desired compiler (e.g. 'clang').

To *temporarily* set or override the `CC` environment variable for the duration of emblob's execution:

~~~sh
env CC=<compiler> emblob <args>
~~~
