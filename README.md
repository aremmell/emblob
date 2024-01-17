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
    - [Linker object input](#linker-object-input)
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

<!--The CMake configuration is two-stage; it compiles emblob in the `build` directory, then it *executes emblob* with a test input file, `example/example.bin`. This is a 15-byte binary file with the values 0x01 through 0x0f. In response, emblob generates several files:

- `example.S`: A linker assembly file containing instructions for the linker to embed `example/example.bin` into `example.o`
- `example.o`: A linker input object file which contains `example/example.bin` as a binary blob
- `emblob_example.h`: A C/C++ header file containing routines to access binary blob data. -->

#### <a id="generated-code" /> Generated code

One of emblob's build products is a C and C++ compatible header file containing generated code that provides quick and easy access to the related embedded blob's data at runtime. The name of the header is dependent upon the value passed to emblob on the command-line for the `--outfile/-o` option. If you do not specify a value for this option, the basename of the input file is used. The header file's name will always be in the format `emblob_{outfile}.h`:

- `emblob -i gorilla.html` &rightarrow; `emblob_gorilla.h`
- `emblob -i gorilla.html -o chimpanzee` &rightarrow; `emblob_chimpanzee.h`

##### <a id="generated-functions" /> Functions

All generated functions have names in the format `emblob_get_{outfile}_{what}` where `{what}` represents the function's specific task or return type. All functions are declared `static inline` and `extern "C"`.

1.
   ```cpp
   uint64_t emblob_get_{outfile}_size()
   ```

   Returns the size of the embedded blob, in bytes.
2.
   ```cpp
   const uint8_t* emblob_get_{outfile}_8()
   ```

   Returns a pointer to the embedded blob that may be used to access the blob's data *one byte (8-bits)* at a time.
3.
   ```cpp
   const uint16_t* emblob_get_{outfile}_16()
   ```

   Returns a pointer to the embedded blob that may be used to access the blob's data *two bytes (16-bits)* at a time.
4.
   ```cpp
   const uint32_t* emblob_get_{outfile}_32()
   ```

   Returns a pointer to the embedded blob that may be used to access the blob's data *four bytes (32-bits)* at a time.
5.
   ```cpp
   const uint64_t* emblob_get_{outfile}_64()
   ```

   Returns a pointer to the embedded blob that may be used to access the blob's data *eight bytes (64-bits)* at a time.
6.
   ```cpp
   const void* emblob_get_{outfile}_raw()
   ```

   Returns a pointer to the embedded blob that may be used to access the blob's data arbitrarily.

#### <a id="#linker-object-input /> Linker object input

Last but not least, emblob generates a linker object input (*.o*) file. As is the case with the generated header, its name is derived from the `--outfile/-o` option and has the format `{outfile}.o`. This is the file that physically contains the contents of the embedded blob, and it must become part of your executable in order to be useful.

Your build system likely has its own unique syntax and structure for achieving this, but for the purposes of this document, I will simply demonstrate how this can be done manually using a compiler frontend such as GCC or Clang:

Given a source code file named `my_application.cpp`, and a linker object input file generated by emblob named `blob.o`, the following commands will:

1. Compile the source code file and generate a linker object input file named `my_application.o`
2. Link together `my_application.o` and `blob.o` into an executable named `my_application`
3. Run the new executable

```sh
c++ -c my_application.cpp && c++ -o my_application my_application.o blob.o && ./my_application
```

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

The C++ source code for the following example programs can be found in the `examples` directory. I used this free online [hex editor](https://hexed.it/) to create the example input files, but any old hex editor will do (*or you can even create programs to generate them*).

### <a id="simple" /> Simple

The simplest use case scenario: input is a binary file containing just 15 bytes (with values 0x01 through 0x0f). The program prints the size of the embedded blob in bytes, then the value of each byte in hexadecimal format. [Source code](https://github.com/aremmell/emblob/blob/master/examples/simple.cc)

### <a id="data-structures" /> Data structures

A particularly useful side effect of the C language (*and by extension, C++*) is the ability to directly map the contents of an embedded blob to a type-safe data structure and vice versa&mdash;a data structure may be serialized to a file quite easily. This example program demonstrates how you can effortlessly create a custom binary file, embed it as a blob, obtain a pointer to its data, cast to an appropriate type, and access members as if the data structure were initialized at compile-time. [Source code](https://github.com/aremmell/emblob/blob/master/examples/struct.cc)

> Note: this example does not take into account the [endianness](https://en.wikipedia.org/wiki/Endianness#Byte_addressing) of the system it is running on&mdash;the example file is in little-endian format.

## <a id="using-specific-compiler" /> Using a specific compiler frontend

When choosing a compiler frontend, emblob will attempt to read the `CC` environment variable. If it is empty, emblob will execute `cc`.

In order to choose a specific compiler frontend, simply set the `CC` environment variable to the name of the desired compiler (e.g. 'clang').

To *temporarily* set or override the `CC` environment variable for the duration of emblob's execution:

```sh
env CC={compiler} emblob {args}
```
