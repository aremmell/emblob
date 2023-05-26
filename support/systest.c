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

bool check_filesystem_api(char* thisfile) {
    bool all_passed = true;

    printf("thisfile = '%s'\n", thisfile);

    char buf[SYSTEST_MAXPATH] = {0};
    all_passed &= systest_getcwd(buf, SYSTEST_MAXPATH);
    printf("cwd = '%s'\n", buf);

    char* bname = systest_getbasename(thisfile);
    printf("basename = '%s'\n", bname);
    all_passed &= 0 != strcmp(bname, ".");

    char* dir = systest_getdirname(thisfile);
    printf("dirname = '%s'\n", dir);
    all_passed &= 0 != strcmp(bname, ".");

    char* appdir = systest_getappdir();
    printf("appdir = '%s'\n", appdir);

    return all_passed;
}

int main(int argc, char *argv[]) {
#if !defined(_WIN32)
#if defined(__STDC_LIB_EXT1__)
    printf("__STDC_LIB_EXT1__ is set!\n");
#endif
#else
#if defined(__STDC_SECURE_LIB__)
    printf("__STDC_SECURE_LIB__ is set!\n");
#endif
#endif

    printf("\t" BLUE("~~~~~~~~~~ <systest> ~~~~~~~~~~") "\n");

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
    ret = check_filesystem_api(argv[0]);
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
    if (!dir)
        handle_error(EINVAL, "dir is invalid")
        return false;

    if (0 == size || size > SYSTEST_MAXPATH) {
        handle_error(EINVAL, "size is invalid")
        return false;
    }

#if !defined(_WIN32)
    char* cwd = getcwd(dir, size);
    if (NULL == cwd) {
        handle_error(errno, "getcwd() failed");
        return false;
    } else {
        printf("getcwd() = '%s'\n", cwd);
    }
#else
    if (NULL == _getcwd(dir, (int)size)) {
        handle_error(errno, "_getcwd() failed");
        return false;
    }
#endif

    return true;
}

char* systest_getbasename(char* path) {
    if (!path || !*path)
        return ".";

#if !defined(_WIN32)
    /* https://www.man7.org/linux/man-pages/man3/basename.3.html

       The functions dirname() and basename() break a null-terminated
       pathname string into directory and filename components.  In the
       usual case, dirname() returns the string up to, but not
       including, the final '/', and basename() returns the component
       following the final '/'.  Trailing '/' characters are not counted
       as part of the pathname.

       If path does not contain a slash, dirname() returns the string
       "." while basename() returns a copy of path.  If path is the
       string "/", then both dirname() and basename() return the string
       "/".  If path is a null pointer or points to an empty string,
       then both dirname() and basename() return the string ".".    
    */
    return basename(path);    
#else
    return PathFindFileNameA(path);
#endif
}

char* systest_getdirname(char* path) {
    if (!path || !*path)
        return ".";

#if !defined(_WIN32)
    return dirname(path);
#else
    HRESULT ret = PathCchRemoveFileSpec(path, strnlen(path, SIR_MAXPATH));
    if (S_OK != ret) {
        handle_error(ret, "PathCchRemoveFileSpec() failed");

    return path;
#endif
}

char* systest_getappdir(void) {

#if !defined(_WIN32)
    /* TBD. */
    return ".";
#else
    char filename[SIR_MAXPATH] = {0};
    DWORD ret = GetModuleFileName(NULL, filename, SIR_MAXPATH);
    if (0 == ret) {
        handle_error(GetLastError(), "GetModuleFileName() failed");
        return ".";
    }
    return _sir_getdirname(filename);
#endif        
}

//
// utility functions
//
void _handle_error(int err, const char* msg, char* file, int line, const char* func) {
    fprintf(stderr, RED("ERROR: %s:%d in %s: error = '%s' (%s)") "\n", systest_getbasename(file), line, func, strerror(err), msg);
}

void _handle_problem(const char* msg, char* file, int line, const char* func) {
    fprintf(stderr, RED("ERROR: %s:%d in %s: %s") "\n", systest_getbasename(file), line, func, msg);
}

