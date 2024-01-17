# emblob

<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com> -->

[![License](https://img.shields.io/github/license/aremmell/emblob?color=%2340b900&cacheSeconds=60)](https://github.com/aremmell/emblob/blob/master/LICENSE)
[![REUSE status](https://api.reuse.software/badge/github.com/aremmell/emblob)](https://api.reuse.software/info/github.com/aremmell/emblob)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=aremmell_emblob&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=aremmell_emblob)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=aremmell_emblob&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=aremmell_emblob)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=aremmell_emblob&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=aremmell_emblob)

A CLI tool to be used as part of a build toolchain which generates linker input object files containing arbitrary files as binary objects (blobs) with accompanying C/C++ header files providing the necessary code to access the blob's data at runtime.

<!-- toc -->

- [Building from source](#build-from-source)
  - [Build products](#build-products)
- [Command-line interface](#cli-interface)
  - [Options](#cli-options)
  - [Examples](#cli-examples)
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

The CMake configuration is designed to be multi-purpose; it compiles emblob as an executable in the `build` directory, then it executes emblob with a test input file, `test.bin`. This is a 250-byte binary file with the first 15 bytes set to 0x1 through 0xf and the rest zeroes. emblob generates several files:

- `test.S`: A linker assembly file containing instructions for the linker to embed `test.bin` into `test.o`
- `test.o`: A linker input object file which contains `test.bin` as a binary blob
- `emblob_test.h`: A C/C++ header file containing routines to access binary blob data.

Following this, two example executables are compiled from the corresponding source files in the `example` directory and linked with `test.o`. They are placed in the `build` directory, and are named `cexample` and `cxxexample`.

These programs are bare-bones demonstrations of basic usage of the emblob-generated files. When run, they perform an iteration of the first 15 bytes in the embedded binary blob and print the hexadecimal values to stdout.

# <a id="cli-interface" /> Command-line interface

emblob's command-line interface is quite simple:

## <a id="cli-options" /> Options

| Name | Short name | Description | Default value |
|:-----------|:-----|:------------|:-------------:|
| `--infile` | `-i` | The relative path of the file to embed as a binary blob. | N/A |
| `--outfile` | `-o` | The *basename* of the output files (e.g. 'foo' will result in foo.S, foo.o, and emblob_foo.h). | Basename of the input file |
| `--log-level` | `-l` | Sets the console logging verbosity: [debug, info, warning, error, fatal]. | info |
| `--version` | `-v` | Prints emblob version information. | N/A |
| `--help` | `-h` | Prints emblob usage information. | N/A |

## <a id="cli-examples" /> Examples

### Bare minimum

At the minimum, emblob requires the input file name. This will use defaults for all other options:

~~~
emblob --infile test.bin
~~~

## <a id="using-specific-compiler" /> Using a specific compiler frontend

When choosing a compiler frontend, emblob will attempt to read the `CC` environment variable. If it is empty, emblob will execute `cc`.

In order to choose a specific compiler frontend, simply set the `CC` environment variable to the name of the desired compiler (e.g. 'clang').

To *temporarily* set or override the `CC` environment variable for the duration of emblob's execution:

~~~
env CC=<compiler> emblob <args>
~~~
