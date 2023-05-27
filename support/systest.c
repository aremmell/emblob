#include "systest.h"

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
    char tmp_path[SYSTEST_MAXPATH] = {0};
    all_passed &= systest_getappfilename(tmp_path, SYSTEST_MAXPATH);
    printf(WHITE("systest_getappfilename() = '%s'") "\n", tmp_path);
    /* ==== */

    /* ==== get base name (file name component of a path)  ==== */
    char* basenametest = strdup(tmp_path);
    if (!basenametest) {
        handle_error(errno, "strdup() failed!");
        return false;
    }

    char* basenameresult = systest_getbasename(basenametest);
    all_passed &= (strlen(basenameresult) > 0 && 0 != strcmp(basenameresult, "."));
    printf(WHITE("systest_getbasename() = '%s'") "\n", basenameresult);
    free(basenametest);
    basenametest = NULL;
    /* ==== */

    /* ==== get dir name (directory structure component of a path)  ==== */
    char* dirnametest = strdup(tmp_path);
    if (!dirnametest) {
        handle_error(errno, "strdup() failed!");
        return false;
    }

    char* dirnameresult = systest_getdirname(dirnametest);
    all_passed &= (strlen(dirnameresult) > 0 && 0 != strcmp(dirnameresult, ".")); 
    printf(WHITE("systest_getdirname() = '%s'") "\n", dirnameresult);
    free(dirnametest);
    dirnametest = NULL;    
    /* ==== */

    /* ==== get app dir path (absolute path of directory containing binary file) ==== */
    memset(tmp_path, '\0', SYSTEST_MAXPATH);    
    all_passed &= systest_getappdir(tmp_path, SYSTEST_MAXPATH);
    printf(WHITE("systest_getappdir() = '%s'") "\n", tmp_path);
    /* ==== */

    /* ==== get current working directory (not necessarily the same ass app directory) ==== */
    memset(tmp_path, '\0', SYSTEST_MAXPATH);    
    all_passed &= systest_getcwd(tmp_path, SYSTEST_MAXPATH);
    printf(WHITE("systest_getcwd() = '%s'") "\n", tmp_path);
    /* ==== */
    
    return all_passed;
}

void check_build_env() {
#if !defined(_WIN32)
#if defined(__STDC_LIB_EXT1__)
    printf("__STDC_LIB_EXT1__ is defined\n");
#else
    printf("__STDC_LIB_EXT1__ NOT defined\n");
#endif
#if defined(__GLIBC__)
    printf("Using GNU libc version: %u.%u\n" __GLIBC__, __GLIBC__MINOR__)
#else
    printf("Not using GNU libc\n");
#endif
#if defined(__linux__) && defined(__HAVE_UNISTD_READLINK__)
    printf("__HAVE_UNISTD_READLINK__ is defined\n");
#else
    printf("__HAVE_UNISTD_READLINK__ NOT defined\n");
#endif
#else // _WIN32
#if defined(__STDC_SECURE_LIB__)
    printf("__STDC_SECURE_LIB__ is defined\n");
#else
    printf("__STDC_SECURE_LIB__ NOT defined\n");
#endif
#endif    
}

void check_platform()
{
#if !defined(_WIN32)
#else
#endif
}

int main(int argc, char *argv[]) {

    printf("\t" BLUE("~~~~~~~~~~ <systest> ~~~~~~~~~~") "\n");

    //
    // begin environment tests
    //

    check_build_env();
    check_platform();

    //
    // begin feature tests
    //

    // popen() and pclose()
    bool ret = check_sysconf(_SC_2_VERSION, "popen() and pclose()");
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

    //
    // begin portability tests
    //

    /* file existence: one file we know exists, and one we know doesn't. */
    const char* exists = "../../LICENSE";
    const char* doesntexist = "a_s_d_f_foobar.baz";

    /* these will assert() and log errors to the console if they are wrong, too. */
    bool exist1 = file_exists(exists, true);
    bool exist2 = file_exists(doesntexist, false);

    handle_result(exist1 && !exist2, "file_exists");

    if (num_succeeded != num_attempted)
        printf("\t" WHITE("--- %d of %d tests passed ---\n"), num_succeeded, num_attempted);
    else
        printf("\t" GREEN("--- all %d tests passed! ---\n"), num_attempted);

    printf("\t" BLUE("~~~~~~~~~~ </systest> ~~~~~~~~~~") "\n");

    return num_succeeded > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}


//
// portability test implementations
//


bool file_exists(const char* path, bool really_exists) {

    if (!path || !*path) {
        handle_error(EINVAL, "path is not a valid string");
        return false;
    }

    bool retval = false;

#if defined(_WIN32)
    retval = (TRUE == PathFileExists(path));
#else
    struct stat st = {0};
    int ret = stat(path, &st);
    if (0 != ret) {
        if (ENOENT != errno) {
            handle_error(errno, "stat(): ret != 0 && ENOENT != errno");
        } else {
            printf("stat() returned != 0, but errno = %d (%s)\n", errno, strerror(errno));
        }
        retval = false;
    } else {
        retval = true;
    }
#endif

    if (retval != really_exists) {
        handle_problem("file: %s, really_exists: %s, errno: %s", path, bool_to_str(really_exists),
            strerror(errno));
        assert(retval == really_exists);
    }

    return retval;
}

bool systest_getcwd(char* restrict dir, size_t size) {
    if (!dir) {
        handle_error(EINVAL, "dir is NULL")
        return false;
    }

    if (size < SYSTEST_MAXPATH) {
        handle_error(EINVAL, "size is < SYSTEST_MAXPATH")
        return false;
    }

#if !defined(_WIN32)
#   if defined(__linux__) && defined(_GNU_SOURCE)
    char* cur = get_current_dir_name();
    strncpy(dir, cur, strnlen(cur, SYSTEST_MAXPATH));
#   else
    char* cwd = getcwd(dir, size);
    if (NULL == cwd) {
        handle_error(errno, "getcwd() failed");
        return false;
    }
    return true;
#   endif
#elif defined(__BSD__)
#else
    /* _WIN32 */
    if (NULL == _getcwd(dir, (int)size)) {
        handle_error(errno, "_getcwd() failed");
        return false;
    }
#endif

    return false;
}

bool systest_getappfilename(char* buffer, size_t size) {
    // TODO: come up with some minimal fallback; at least look at argv[0] if
    // the method chosen doesn't work.
    if (!buffer) {
        handle_error(EINVAL, "buffer is NULL")
        return false;
    }

    if (size < SYSTEST_MAXPATH) {
        handle_error(EINVAL, "size is < SYSTEST_MAXPATH")
        return false;
    }

    bool retval = false;

#if !defined(_WIN32)
#   if (defined(__GLIBC__) && defined(__HAVE_UNISTD_READLINK__)) || defined(__BSD__)
    ssize_t read = readlink("/proc/self/exe", buffer, size);
    if (-1 == read) {
        handle_error(errno, "readlink() failed!");
        retval = false;
    } else if (read > (ssize_t)size) {
        handle_error(ENOBUFS, "readlink() failed (buffer too small)!");
        retval = false;
    }
#   else
#   error "no readlink(); don't have an implementation for " __func__
#   endif
#elif defined(__APPLE__)
    uint32_t size32 = (uint32_t)size;
    if (0 != _NSGetExecutablePath(buffer, &size32)) {
        /* buffer is too small; need size32 bytes */
        handle_error(ENOBUFS, "_NSGetExecutablePath() failed (buffer too small)!");
        retval = false;
    } else {
        retval = true;
    }
#else
    /* _WIN32 */
    if (0 == GetModuleFileName(NULL, buffer, size)) {
        handle_error(GetLastError(), "GetModuleFileName() failed");
        retval = false;
    } else {
        retval = true;
    }
#endif

    return retval;
}

bool systest_getappdir(char* restrict buffer, size_t size) {
    char tmp[SYSTEST_MAXPATH] = {0};
    if (systest_getappfilename(tmp, SYSTEST_MAXPATH)) {
        strncpy(buffer, systest_getdirname(tmp), strnlen(tmp, size));
        return true;
    }

    return false;
}

char* systest_getbasename(char* restrict path) {
    if (!path || !*path) {
        handle_error(EINVAL, "path is an invalid string")
        return ".";
    }

    if (path[0] == '/' && path[1] == '\0')
        return "/";

#if !defined(_WIN32)
    return basename(path);
#else
    return PathFindFileNameA(path);
#endif
}

char* systest_getdirname(char* restrict path) {
    if (!path || !*path) {
        handle_error(EINVAL, "path is an invalid string")
        return ".";
    }

    if (path[0] == '/' && path[1] == '\0')
        return "/";

#if !defined(_WIN32)
    return dirname(path);
#else
    HRESULT ret = PathCchRemoveFileSpec(path, strnlen(path, SIR_MAXPATH));
    if (S_OK != ret) {
        handle_error(ret, "PathCchRemoveFileSpec() failed!");
    }

    return path;
#endif
}


//
// utility functions
//


void _handle_error(int err, const char* msg, char* file, int line, const char* func) {
    fprintf(stderr, RED("ERROR: %s:%d in %s: error = '%s' (%s)") "\n", file, line, func, strerror(err), msg);
}

void _handle_problem(const char* msg, char* file, int line, const char* func) {
    fprintf(stderr, RED("ERROR: %s:%d in %s: %s") "\n", file, line, func, msg);
}

