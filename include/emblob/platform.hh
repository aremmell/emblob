/*
 * platform.hh
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2024
 * Version:   2.0.0
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
#ifndef _EMBLOB_PLATFORM_HH_INCLUDED
# define _EMBLOB_PLATFORM_HH_INCLUDED

# if defined(__APPLE__) && defined(__MACH__)
#  define __MACOS__
#  define _DARWIN_C_SOURCE
#  define EMBLOB_PLATFORM macOS
# elif defined(__linux__)
#  define __LINUS__
#  define EMBLOB_PLATFORM Linux
# elif defined(_WIN32)
#  define EMBLOB_PLATFORM Windows
# elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#  define __BSD__
#  define EMBLOB_PLATFORM BSD
#  if !defined(_GNU_SOURCE)
#   define _GNU_SOURCE
#  endif
# elif defined(_WIN32)
#  define __WIN__
#  define EMBLOB_PLATFORM Windows
# elif defined(__FreeBSD__)
#  define __BSD__
#  define EMBLOB_PLATFORM BSD
#  define _BSD_SOURCE
#  if !defined(_DEFAULT_SOURCE)
#   define _DEFAULT_SOURCE
#  endif
# else
#  error "Unable to determine platform; please contact the author."
# endif

# if !defined(_WIN32)
#  if defined(__STDC_WANT_LIB_EXT1__)
#   undef __STDC_WANT_LIB_EXT1__
#  endif
#  define __STDC_WANT_LIB_EXT1__ 1
# else
#  if defined(__WANT_STDC_SECURE_LIB__)
#   undef __WANT_STDC_SECURE_LIB__
#  endif
#  define __WANT_STDC_SECURE_LIB__ 1
# endif

# if !defined (_WIN32)
#  include <sys/wait.h>
#  include <unistd.h>
# else
#  define WIN32_LEAN_AND_MEAN
#  define WINVER 0x0A00
#  define _WIN32_WINNT 0x0A00
#  include <windows.h>
# endif

# include <exception>
# include <functional>
# include <fstream>
# include <sstream>
# include <iostream>
# include <memory>
# include <vector>
# include <array>
# include <regex>
# include <limits>
# include <string>
# include <string_view>
# include <ranges>
# include <cstdlib>
# include <cstdarg>
# include <cstdint>
# include <cctype>
# include <sys/types.h>
# include <sys/stat.h>
# include <stdio.h>
# include <errno.h>
# include <string.h>
# include <inttypes.h>
# include <assert.h>

# if !defined(_WIN32) && defined(__STDC_LIB_EXT1__)
#  define __HAVE_STDC_SECURE_OR_EXT1__
# elif defined(__STDC_SECURE_LIB__)
#  define __HAVE_STDC_SECURE_OR_EXT1__
# endif

# if defined(__MACOS__) || defined(__BSD__) || \
    (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L && !defined(_GNU_SOURCE)))
#  define __HAVE_XSI_STRERROR_R__
#  if defined(__GLIBC__)
#   if (__GLIBC__ >= 2 && __GLIBC_MINOR__ < 13)
#    define __HAVE_XSI_STRERROR_R_ERRNO__
#   endif
#  endif
# elif defined(_GNU_SOURCE) && defined(__GLIBC__)
#  define __HAVE_GNU_STRERROR_R__
# elif defined(__HAVE_STDC_SECURE_OR_EXT1__)
#  define __HAVE_STRERROR_S__
#  define __HAVE_FOPEN_S__
# endif

# define MAX_ERRORMSG 256

#endif // ! _EMBLOB_PLATFORM_HH_INCLUDED

