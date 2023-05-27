#ifndef _SYSTEST_H_INCLUDED
#define _SYSTEST_H_INCLUDED

#if !defined(_WIN32)
#   if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#       define __BSD__
#       define _BSD_SOURCE
#   endif

#   define _DEFAULT_SOURCE
#   define _GNU_SOURCE
#   define _POSIX_C_SOURCE 200809L
#   define _XOPEN_SOURCE 700
#   define __STDC_WANT_LIB_EXT1__ 1

#   if (defined(__GLIBC__) && (__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19 && defined(_BSD_SOURCE)))
#       define __HAVE_UNISTD_READLINK__
#   endif

#   include <sys/types.h>
#   include <sys/stat.h>
#   include <unistd.h>
#   include <libgen.h>
#   include <limits.h>
#   include <fcntl.h>

#   if defined(PATH_MAX)
#       define SYSTEST_MAXPATH PATH_MAX
#   else
#       define SYSTEST_MAXPATH 1024
#   endif

#   define STRFMT(clr, s) clr s "\033[0m"
#   define RED(s) STRFMT("\033[1;91m", s)
#   define GREEN(s) STRFMT("\033[1;92m", s)
#   define WHITE(s) STRFMT("\033[1;97m", s)
#   define BLUE(s) STRFMT("\033[1;34m", s)
#else
#   define __WANT_STDC_SECURE_LIB__ 1
#   define WIN32_LEAN_AND_MEAN
#   define WINVER       0x0A00
#   define _WIN32_WINNT 0x0A00
#   include <windows.h>
#   include <shlwapi.h>
#   include <pathcch.h>

#   define SYSTEST_MAXPATH MAX_PATH

#   define RED(s) s
#   define GREEN(s) s
#   define WHITE(s) s
#   define BLUE(s) s
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>

#if defined(__APPLE__)
#   include <mach-o/dyld.h>
#elif defined(__FreeBSD__)
#   include <sys/sysctl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// portability test implementations
//
bool file_exists(const char* path, bool really_exists);

bool systest_getcwd(char* restrict buffer, size_t size);
bool systest_getappfilename(char* restrict buffer, size_t size);
bool systest_getappdir(char* restrict buffer, size_t size);
char* systest_getbasename(char* restrict path);
char* systest_getdirname(char* restrict path); 

//
// utility functions
//

/* this is strictly for use when encountering an actual failure of a system call. 
 * use handle_problem to report things other than error numbers. */
void _handle_error(int err, const char* msg, char* file, int line, const char* func);
#define handle_error(err, msg) _handle_error(err, msg, __FILE__, __LINE__, __func__);

void _handle_problem(const char* msg, char* file, int line, const char* func);

static char _handle_problem_buf[512] = {0};
#define handle_problem(problem, ...)  \
    snprintf(_handle_problem_buf, 512, problem, __VA_ARGS__); \
    _handle_problem(_handle_problem_buf, __FILE__, __LINE__, __func__);

/* converts bool -> const char* */
#define bool_to_str(b) b ? "true" : "false"

#ifdef __cplusplus
}
#endif

#endif // !_SYSTEST_H_INCLUDED
