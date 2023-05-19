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
#if defined(SYSTEST_USE_ACCESS)
    int ret = access(path, F_OK);
    if (0 != ret) {
        retval = false;
    } else {
        retval = true;
    }
#elif defined(SYSTEST_USE_STAT)
    struct stat st = {0};
    int ret = stat(path, &st);
    if (0 != ret) {
        if (ENOENT != errno) {
            handle_error(errno, "stat(): ret != 0 && ENOENT != errno");
        } else {
            printf("stat() returned != 0, but errno = %s\n", strerror(errno));
        }
        retval = false;
    } else {
        retval = true;
    }
#endif
#endif

    if (retval != really_exists) {
        handle_problem("file: %s, really_exists: %s, errno: %s", path, bool_to_str(really_exists),
            strerror(errno));
        assert(retval == really_exists);
    }

    return retval;
}

//
// utility functions
//
void _handle_error(int err, const char* msg, const char* file, int line, const char* func) {
    fprintf(stderr, RED("ERROR: %s:%d in %s: error = '%s' (%s)") "\n", get_basename(file), line, func, strerror(err), msg);
}

void _handle_problem(const char* msg, const char* file, int line, const char* func) {
    fprintf(stderr, RED("ERROR: %s:%d in %s: %s") "\n", get_basename(file), line, func, msg);
}

const char* get_basename(const char* filename) {
#if defined(_WIN32)
    return PathFindFileName(filename);
#else
    return basename((char*)filename);
#endif
}
