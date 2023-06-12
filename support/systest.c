#include "systest.h"

#if defined(_WIN32)
#   pragma comment(lib, "Shlwapi.lib")
#endif

int num_attempted = 0;
int num_succeeded = 0;

void handle_result(bool pass, const char* desc) {
    /* print something reliable, so that if we need to use tools
     * to parse the output of this program to determine support,
     * it will be much easier. */
    if (pass) {
        printf("\t" GREEN("PASS: %s") "\n", desc);
        num_succeeded++;
    } else {
        fprintf(stderr, "\t" RED("FAIL: %s") "\n", desc);
    }

    num_attempted++;
}

#if !defined(_WIN32)
bool check_sysconf(int val, const char* desc) {
    printf("checking sysconf(%d) (\"%s\")...\n", val, desc);

    long ret = sysconf(val);
    if (ret == -1) {
        printf("sysconf(%d) error: %s!\n", val, strerror(errno));
        return false;
    } else {
        printf("sysconf(%d) = %ld.\n", val, ret);
        return true;
    }

    return ret;
}
#endif

bool check_popen() {
#if !defined(_WIN32)
    return check_sysconf(_SC_2_VERSION, "popen() and pclose()");
#else
    /* https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/popen-wpopen */
    printf("windows has _popen()/_pclose().\n");
    return true;
#endif
}

bool check_system() {
    printf("checking system(NULL)...\n");

    if (0 == system(NULL)) {
        printf("system() is NOT available to execute commands!\n");
        return false;
    } else {
        printf("system() is available to execute commands.\n");
        return true;
    }
}

bool check_z_printf() {
    char buf[256] = {0};
    size_t n = 10;
    snprintf(buf, 256, "printing a size_t with the value ten: '%zu'", n);
    printf("%s\n", buf);

    if (NULL != strstr(buf, "10")) {
        printf("Found '10' in the format string.\n");
        return true;
    }

    return false;
}

bool check_filesystem_api(void) {
    bool all_passed = true;

    /* ==== get app file name path (absolute path of binary file) ==== */
    char* appfilename = systest_getappfilename();
    all_passed &= _validptr(appfilename);
    if (_validptr(appfilename)) {
        printf(WHITE("systest_getappfilename() = '%s'") "\n", appfilename);
        /* ==== */

        /* ==== get base name (file name component of a path)  ==== */
        char* basenametest = strdup(appfilename);
        if (!basenametest) {
            handle_error(errno, "strdup() failed!");
            return false;
        }

        char* basenameresult = systest_getbasename(basenametest);
        all_passed &= (strlen(basenameresult) > 0 && 0 != strcmp(basenameresult, "."));
        printf(WHITE("systest_getbasename() = '%s'") "\n", basenameresult);
        systest_safefree(basenametest);
        /* ==== */

        /* ==== get dir name (directory structure component of a path)  ==== */
        char* dirnametest = strdup(appfilename);
        if (!dirnametest) {
            handle_error(errno, "strdup() failed!");
            return false;
        }

        char* dirnameresult = systest_getdirname(dirnametest);
        all_passed &= (strlen(dirnameresult) > 0 && 0 != strcmp(dirnameresult, ".")); 
        printf(WHITE("systest_getdirname() = '%s'") "\n", dirnameresult);
        systest_safefree(dirnametest);
        /* ==== */

        systest_safefree(appfilename);
    } else {
        printf(RED("systest_getbasename() = skipped") "\n");
        printf(RED("systest_getdirname() = skipped") "\n");
    }

    /* ==== get app dir path (absolute path of directory containing binary file) ==== */
    char* appdir = systest_getappdir();
    all_passed &= _validptr(appdir);
    printf(WHITE("systest_getappdir() = '%s'") "\n", prn_str(appdir));
    systest_safefree(appdir);
    /* ==== */

    /* ==== get current working directory (not necessarily the same ass app directory) ==== */
    char* cwd = systest_getcwd();
    all_passed &= _validptr(cwd);
    printf(WHITE("systest_getcwd() = '%s'") "\n", prn_str(cwd));
    systest_safefree(cwd);
    /* ==== */

    /* file existence: some we know exist, and some we know don't. */
    static const struct { const char* const path; bool exists; } real_or_not[] = {
        {"../../LICENSE", true},
        {"./i_exist", true},
        {"a_s_d_f_foobar.baz", false},
        {"idontexist", false},
    };

    for (size_t n = 0; n < (sizeof(real_or_not) / sizeof(real_or_not[0])); n++) {
        bool exists = false;
        bool ret    = systest_pathexists(real_or_not[n].path, &exists,
            SYSTEST_PATH_REL_TO_APP);
        all_passed &= ret;
        if (!ret)
            continue;            

        if (exists != real_or_not[n].exists) {
            all_passed = false;
            printf("\t" RED("systest_pathexists('%s') = %s") "\n",
                real_or_not[n].path, exists ? "true" : "false");            
        } else {
            printf("\t" GREEN("systest_pathexists('%s') = %s") "\n",
                real_or_not[n].path, exists ? "true" : "false");               
        }
    }
    
    return all_passed;
}

void check_build_env() {
#if !defined(_WIN32)
# if defined(__STDC_LIB_EXT1__)
   printf("__STDC_LIB_EXT1__ is defined\n");
# else
   printf("__STDC_LIB_EXT1__ NOT defined\n");
# endif
# if defined(__GLIBC__)
    printf("Using GNU libc version: %d.%d\n", __GLIBC__, __GLIBC_MINOR__);
# else
    printf("Not using GNU libc\n");
# endif
#else // _WIN32
# if defined(__STDC_SECURE_LIB__)
   printf("__STDC_SECURE_LIB__ is defined\n");
# else
   printf("__STDC_SECURE_LIB__ NOT defined\n");
# endif
#endif
#if defined(__clang__)
    printf("Using clang\n");
#elif defined(__GNUC__)
    printf("Using GCC\n");
#else
    printf("Using unknown toolset\n");
#endif
}

void check_platform()
{
#if !defined(_WIN32)
#else

#endif
}

void check_safefree() {
    int *ptr = malloc(sizeof(int));
    *ptr = 1234;
    systest_safefree(ptr);

    if (ptr)
        printf("safe_free() does reset the pointer\n");
    else
        printf(RED("safe_free() does NOT reset the pointer!\n"));
}

int main(int argc, char *argv[]) {

    printf("\t" BLUE("~~~~~~~~~~ <systest> ~~~~~~~~~~") "\n");

    //
    // begin environment tests
    //

    check_build_env();
    check_platform();

    //
    // begin curiosity tests
    //

    check_safefree();

    //
    // begin feature tests
    //

    // popen() and pclose()
    bool ret = check_popen();
    handle_result(ret, "popen() and pclose()");

    // system()
    ret = check_system();
    handle_result(ret, "system()");

    // can you use a 'z' prefix in a printf-style specifier for size_t portably?
    ret = check_z_printf();
    handle_result(ret, "z prefix in *printf");

    //
    // these are just for collection of data
    // at the moment. just need to understand
    // their behavior on different platforms.
    //

    ret = check_filesystem_api();
    handle_result(ret, "filesystem api");

    if (num_succeeded != num_attempted)
        printf("\t" WHITE("--- %d/%d tests passed ---\n"), num_succeeded, num_attempted);
    else
        printf("\t" GREEN("--- all %d tests passed! ---\n"), num_attempted);

    printf("\t" BLUE("~~~~~~~~~~ </systest> ~~~~~~~~~~") "\n");

    return num_succeeded > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}


//
// portability test implementations
//

bool systest_pathgetstat(const char* restrict path, struct stat* restrict st, systest_rel_to rel_to) {
    if (!_validstr(path) || !_validptr(st))
        return false;

    memset(st, 0, sizeof(struct stat));

    int stat_ret  = -1;
    bool relative = false;
    if (!systest_ispathrelative(path, &relative))
        return false;
    
    if (relative) {
        char* base_path = NULL;
        switch(rel_to) {
            case SYSTEST_PATH_REL_TO_APP: base_path = systest_getappdir(); break;
            case SYSTEST_PATH_REL_TO_CWD: base_path = systest_getcwd(); break;
            default: self_log("invalid enum!"); return false;
        }

        if (!base_path) {
            handle_error(errno, "couldn't get base path!");
            return false;
        }

#if !defined(__WIN__)
# if defined(__MACOS__)
        int open_flags = O_SEARCH;
# elif defined(__linux__)
        int open_flags = O_PATH | O_DIRECTORY;
# elif defined(__BSD__)
        int open_flags = O_EXEC | O_DIRECTORY;
# endif

        int fd = open(base_path, open_flags);
        if (-1 == fd) {
            handle_error(errno, "open() failed!");
            return false;
        }

        stat_ret = fstatat(fd, path, st, AT_SYMLINK_NOFOLLOW);
        systest_safeclose(&fd);
        systest_safefree(base_path);
    } else {
        stat_ret = stat(path, st);
    }
#else // __WIN__
        char abs_path[SIR_MAXPATH] = {0};
        if (NULL == PathCombineA(abs_path, base_path, path)) {
            handle_error(GetLastError(), "PathCombineA() failed!");
        } else {
            abs_path[SIR_MAXPATH - 1] = '\0';
            stat_ret = stat(abs_path, st);
        }
        systest_safefree(base_path);
    } else {
        stat_ret = stat(path, st);
    }
#endif
    if (-1 == stat_ret) {
        if (ENOENT == errno) {
            st->st_size = SYSTEST_STAT_NONEXISTENT;
            return true;
        } else {
            handle_error(errno, "stat() failed!");
            return false;
        }
    }

    return true;
}

bool systest_pathexists(const char* restrict path, bool* restrict exists, systest_rel_to rel_to) {
    if (!_validstr(path) || !_validptr(exists))
        return false;

    *exists = false;

    struct stat st = {0};
    bool stat_ret  = systest_pathgetstat(path, &st, rel_to);
    if (!stat_ret)
        return false;

    *exists = (st.st_size != SYSTEST_STAT_NONEXISTENT);
    return true;
}

char* systest_getcwd(void) {
#if !defined(_WIN32)
# if defined(__linux__) && defined(_GNU_SOURCE)
    char* cur = get_current_dir_name();
    if (NULL == cur)
        handle_error(errno, "");
    return cur;
# else
    char* cur = getcwd(NULL, 0);
    if (NULL == cur)
        handle_error(errno, "");
    return cur;
# endif
#else // _WIN32
    char* cur = _getcwd(NULL, 0);
    if (NULL == cur)
        handle_error(errno, "");
    return cur;
#endif
}

char* systest_getappfilename(void) {

    char* buffer = (char*)calloc(SYSTEST_MAXPATH, sizeof(char));
    if (NULL == buffer) {
        handle_error(errno, "");
        return NULL;
    }

    bool resolved = false;
    size_t size   = SYSTEST_MAXPATH;
    bool grow     = false;

    do {
        if (grow) {
            self_log("reallocating %zu bytes for buffer and trying again...", size);
            systest_safefree(buffer);

            buffer = (char*)calloc(size, sizeof(char));
            if (NULL == buffer) {
                handle_error(errno, "");
                resolved = false;
                break;
            }

            grow = false;
        }

#if !defined(_WIN32)
# if defined(__linux__)
#  if defined(__HAVE_UNISTD_READLINK__)
        ssize_t read = readlink("/proc/self/exe", buffer, size);
        self_log("readlink() returned: %ld (size = %zu)", read, size);
        if (-1 != read && read < (ssize_t)size) {
            resolved = true;
            break;
        } else {
            if (-1 == read) {
                handle_error(errno, "");
                resolved = false;
                break;
            } else if (read >= (ssize_t)size) {
                /* readlink, like Windows' impl, doesn't have a concept
                * of letting you know how much larger your buffer needs
                * to be; it just truncates the string and returns how
                * many chars it wrote there. */
                size += SYSTEST_PATH_BUFFER_GROW_BY;
                grow = true;
                continue;
            }
        }
#  else
#   error "unable to resolve readlink(); see man readlink and its feature test macro requirements."
#  endif
# elif defined(__FreeBSD__)
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
        int ret = sysctl(mib, 4, buffer, &size, NULL, 0);
        self_log("sysctl() returned: %d (size = %zu)", ret, size);
        if (0 == ret) {
            resolved = true;
            break;
        } else {
            handle_error(errno, "");
            resolved = false;
            break;
        }
# elif defined(__APPLE__)
        int ret = _NSGetExecutablePath(buffer, (uint32_t*)&size);
        if (0 == ret) {
            resolved = true;
            break;
        } else if (-1 == ret) {
            grow = true;
            continue;
        } else {
            handle_error(errno, "");
            resolved = false;
            break;
        }
# else
#  error "no implementation for your platform; please contact the author."
# endif
#else // _WIN32
        DWORD ret = GetModuleFileNameA(NULL, buffer, (DWORD)size);
        self_log("GetModuleFileNameA() returned: %lu (size = %zu)", ret, size);
        if (0 != ret && ret < (DWORD)size) {
            resolved = true;
            break;
        } else {
            if (0 == ret) {
                handle_error(GetLastError(), "GetModuleFileNameA() failed!");
                resolved = false;
                break;
            } else if (ret == (DWORD)size || ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
                /* Windows has no concept of letting you know how much larger
                * your buffer needed to be; it just truncates the string and
                * returns size. So, we'll guess. */
                size += SYSTEST_PATH_BUFFER_GROW_BY;
                grow = true;
                continue;
            }
        }
#endif

    } while (true);

    if (!resolved) {
        systest_safefree(buffer);
        self_log("failed to resolve filename!");
    } else {
        self_log("successfully resolved: '%s'", buffer);
    }

    return buffer;
}

char* systest_getappbasename(void) {
    char* filename = systest_getappfilename();
    if (!_validstr(filename))
        return NULL;

    char* retval = systest_getbasename(filename);
    char* bname  = strdup(retval);

    systest_safefree(filename);
    return bname;
}

char* systest_getappdir(void) {
    char* filename = systest_getappfilename();
    if (!_validstr(filename))
        return NULL;

    char* retval = systest_getdirname(filename);
    char* dirname = strdup(retval);

    systest_safefree(filename);
    return dirname;
}

char* systest_getbasename(char* restrict path) {
    if (!_validstr(path))
        return ".";

#if !defined(_WIN32)
    return basename(path);
#else
    return PathFindFileNameA(path);
#endif
}

char* systest_getdirname(char* restrict path) {
    if (!_validstr(path))
        return ".";

#if !defined(_WIN32)
    return dirname(path);
#else
    if (!PathRemoveFileSpecA((LPSTR)path))
        handle_error(GetLastError(), "PathRemoveFileSpecA() failed!");
    return path;
#endif
}

bool systest_ispathrelative(const char* restrict path, bool* restrict relative) {
    if (!_validstr(path) || !_validptr(relative))
        return false;

#if !defined(_WIN32)
    if (path[0] == '/' || (path[0] == '~' && path[1] == '/'))
        *relative = false;
    else
        *relative = true;
    return true;
#else
    *relative = (TRUE == PathIsRelativeA(path));
    return true;
#endif    
}


//
// utility functions
//


void _handle_error(int err, const char* msg, char* file, int line, const char* func) {
    fprintf(stderr, RED("ERROR: %s (%s:%d): %s (%s)") "\n",
        func, file, line, msg, strerror(err));
}

void _self_log(const char* msg, char* file, int line, const char* func) {
    fprintf(stderr, WHITE("%s (%s:%d): %s") "\n", func, file, line, msg);
}
