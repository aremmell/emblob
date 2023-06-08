#ifndef _SYSTEST_H_INCLUDED
#define _SYSTEST_H_INCLUDED

#if !defined(_WIN32)
# if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#  define __BSD__
#  define _BSD_SOURCE
# endif

#define _DEFAULT_SOURCE
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#define __STDC_WANT_LIB_EXT1__ 1

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <fcntl.h>

# if defined(__GLIBC__)
#  if (__GLIBC__ >= 2 && __GLIBC_MINOR__ > 19)  || \
      (__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19) && defined(_BSD_SOURCE)
#   define __HAVE_UNISTD_READLINK__
#  endif
# endif

# if defined(PATH_MAX)
#  define SYSTEST_MAXPATH PATH_MAX
# else
#  define SYSTEST_MAXPATH 1024
# endif
# define SIR_PATH_SEP '/'
#else // _WIN32
# define __WANT_STDC_SECURE_LIB__ 1
# define WIN32_LEAN_AND_MEAN
# define WINVER       0x0A00
# define _WIN32_WINNT 0x0A00
# include <windows.h>
# include <shlwapi.h>
# include <pathcch.h>
# include <direct.h>

# define SYSTEST_MAXPATH MAX_PATH
# define SIR_PATH_SEP '\\'
#endif

#define STRFMT(clr, s) clr s "\033[0m"
#define RED(s) STRFMT("\033[1;91m", s)
#define GREEN(s) STRFMT("\033[1;92m", s)
#define WHITE(s) STRFMT("\033[1;97m", s)
#define BLUE(s) STRFMT("\033[1;34m", s)
#define CYAN(s) STRFMT("\033[1;36m", s)
#define YELLOW(s) STRFMT("\033[1;33m", s)

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

#if defined(__clang__)
# define __file__ __FILE_NAME__
#elif defined(__GNUC__)
# define __file__ __FILE_NAME__
#else
# define __file__ (strrchr(__FILE__, SIR_PATH_SEP) ? strrchr(__FILE__, SIR_PATH_SEP) + 1 : __FILE__)
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// portability test implementations
//


/** Defines how many characters to grow a buffer by which was deemed too small
 * by a system call (with no information regarding the necessary size). */
#define SYSTEST_PATH_BUFFER_GROW_BY 32

/**
 * Determines if a file or directory exists in the filesystem.
 * Returns false if an error occurs or an argument is invalid. Places the result
 * in \p exists.
*/
bool systest_pathexists(const char* restrict path, bool* restrict exists);

/**
 * Returns the current working directory for the calling process. If an error
 * occurs, returns NULL. Pointers returned must be deallocated with free().
 */
char* systest_getcwd(void);

/**
 * Returns the absolute path of the binary executable file for the calling
 * process. If an error occurs, returns NULL. Pointers returned must be
 * deallocated with free().
 */
char* systest_getappfilename(void);

/**
 * Returns the absolute path of the directory containing the binary file
 * of the calling process (not necessarily the current working directory).
 */
char* systest_getappdir(void);

/**
 * Returns only last component of a path.
 * May return ".", "/", or \p path if no determination can be made.
 */
char* systest_getbasename(char* restrict path);

/**
 * Returns all but the last component of a path.
 * May return "." "/", or \p path if no determination can be made.
*/
char* systest_getdirname(char* restrict path);

/**
 * Determines if a given path is relative (or absolute).
 * Returns false if an error occurs or an argument is invalid. Places the result
 * in \p relative.
 */
bool systest_ispathrelative(const char* restrict path, bool* restrict relative);

//
// utility functions
//

/* this is strictly for use when encountering an actual failure of a system call. 
 * use self_log to report things other than error numbers. */
void _handle_error(int err, const char* msg, char* file, int line, const char* func);
#define handle_error(err, msg) _handle_error(err, msg, __file__, __LINE__, __func__);

void _self_log(const char* msg, char* file, int line, const char* func);

static char _self_log_buf[512] = {0};
#define self_log(...)  \
    snprintf(_self_log_buf, 512, __VA_ARGS__); \
    _self_log(_self_log_buf, __file__, __LINE__, __func__);

#define bool_to_str(b) (b ? "true" : "false")
#define prn_str(str) (str ? str : "NULL")
#define _validstr(str) (NULL != str && '\0' != *str)
#define _validptr(p) (NULL != p)

static inline
void _systest_safefree(void** p) {
    if (!p || (p && !*p))
        return;
    free(*p);
    *p = NULL;
}

static inline
void systest_safefree(void* p) {
    _systest_safefree(&p);
}

#ifdef __cplusplus
}
#endif

#endif // !_SYSTEST_H_INCLUDED
