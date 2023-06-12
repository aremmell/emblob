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
# if (__GLIBC__ >= 2 && __GLIBC_MINOR__ > 19)  || \
     (__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19) && defined(_BSD_SOURCE)
#  define __HAVE_UNISTD_READLINK__
# endif
# endif

# if defined(PATH_MAX)
#  define SYSTEST_MAXPATH PATH_MAX
# else
#  define SYSTEST_MAXPATH 1024
# endif
# define SYSTEST_PATH_SEP '/'
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
# define SYSTEST_PATH_SEP '\\'
#endif

#define STRFMT(clr, s) "\x1b[" clr "m" s "\x1b[0m"
#define RED(s) STRFMT("1;91", s)
#define GREEN(s) STRFMT("1;92", s)
#define WHITE(s) STRFMT("1;97", s)
#define BLUE(s) STRFMT("1;34", s)
#define CYAN(s) STRFMT("1;36", s)
#define YELLOW(s) STRFMT("1;33", s)
#define GRAY(s) STRFMT("1;90", s)
#define ULINE(s) STRFMT("4", s)
#define EMPH(s) STRFMT("3", s)
#define BOLD(s) STRFMT("1", s)

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
# define __MACOS__
# include <mach-o/dyld.h>
#elif defined(__FreeBSD__)
# include <sys/sysctl.h>
#endif

#if defined(__clang__) && defined(__FILE_NAME__)
# define __file__ __FILE_NAME__
#elif defined(__GNUC__) && defined(__FILE_NAME__)
# define __file__ __FILE_NAME__
#else
# define __file__ (strrchr(__FILE__, SYSTEST_PATH_SEP) ? strrchr(__FILE__, SYSTEST_PATH_SEP) + 1 : __FILE__)
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

/** Buffer size, in characters, for struct stat -> string. */
#define SYSTEST_STAT_BUFFER_SIZE 128

/** Special flag to indicate to the caller that the file in question
 * does not exist (systest_pathgetstat). */
#define SYSTEST_STAT_NONEXISTENT ((off_t)0xffffff02)

/** Flags used to specify which directory to use as the base reference for
 * testing relative paths. */
typedef enum {
    SYSTEST_PATH_REL_TO_CWD = 0x0001,
    SYSTEST_PATH_REL_TO_APP = 0x0002
} systest_rel_to;

bool systest_pathgetstat(const char* restrict path, struct stat* restrict st, systest_rel_to rel_to);
bool systest_pathexists(const char* restrict path, bool* restrict exists, systest_rel_to rel_to);

char* systest_getcwd(void);

char* systest_getappfilename(void);
char* systest_getappbasename(void);
char* systest_getappdir(void);

char* systest_getbasename(char* restrict path);

char* systest_getdirname(char* restrict path);

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

static inline
void systest_safeclose(int* restrict fd) {
    if (!fd || (fd && 0 > *fd))
        return;

    if (-1 == close(*fd))
        handle_error(errno, "close() failed!");

    *fd = -1;    
}

#ifdef __cplusplus
}
#endif

#endif // !_SYSTEST_H_INCLUDED
