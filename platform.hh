/*
 * platform.hh
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * License:   The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _MKVEROBJ_PLATFORM_HH_INCLUDED
#define _MKVEROBJ_PLATFORM_HH_INCLUDED

#if defined(__APPLE__)
# define __MACOS__
# define _DARWIN_C_SOURCE
# define MKVEROBJ_PLATFORM macOS
#elif defined(__linux__)
# define __LINUS__
# define MKVEROBJ_PLATFORM Linux
#elif defined(_WIN32)
# define MKVEROBJ_PLATFORM Windows
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
# define __BSD__
# define MKVEROBJ_PLATFORM BSD
# if !defined(_GNU_SOURCE)
#  define _GNU_SOURCE
# endif
#elif defined(_WIN32)
# define __WIN__
# define MKVEROBJ_PLATFORM Windows
#elif defined(__FreeBSD__)
# define __BSD__
# define MKVEROBJ_PLATFORM BSD
# define _BSD_SOURCE
# if !defined(_DEFAULT_SOURCE)
#  define _DEFAULT_SOURCE
# endif
#else
# error "Unable to determine platform; please contact the author."
#endif

#if !defined(_WIN32)
#if defined(__STDC_WANT_LIB_EXT1__)
# undef __STDC_WANT_LIB_EXT1__
# endif
# define __STDC_WANT_LIB_EXT1__ 1
#else
# if defined(__WANT_STDC_SECURE_LIB__)
# undef __WANT_STDC_SECURE_LIB__
# endif
# define __WANT_STDC_SECURE_LIB__ 1
#endif

#if !defined(restrict)
# if defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
# define restrict __restrict
# elif defined(__GNUC__)
# define restrict __restrict__
#else
# define restrict
# endif
#endif

#if !defined (_WIN32)
# include <sys/wait.h>
# include <unistd.h>
#else
# define WIN32_LEAN_AND_MEAN
# define WINVER 0x0A00
# define _WIN32_WINNT 0x0A00
# include <windows.h>
#endif

#include <functional>
#include <vector>
#include <regex>
#include <limits>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <cstdint>
#include "util.hh"

#if !defined(_WIN32) && defined(__STDC_LIB_EXT1__)
# define __HAVE_STDC_SECURE_OR_EXT1__
#elif defined(__STDC_SECURE_LIB__)
# define __HAVE_STDC_SECURE_OR_EXT1__
#endif

#if defined(__MACOS__) || defined(__BSD__) || (_POSIX_C_SOURCE >= 200112L && ! _GNU_SOURCE)
# define __HAVE_XSI_STRERROR_R__
#elif defined(_GNU_SOURCE)
# define __HAVE_GNU_STRERROR_R__
#elif defined(__HAVE_STDC_SECURE_OR_EXT1__)
# define __HAVE_STRERROR_S__
# define __HAVE_FOPEN_S__
#endif

#define MAX_ERRORMSG 256

#endif // ! _MKVEROBJ_PLATFORM_HH_INCLUDED

