#ifdef __STDC_LIB_EXT1_CHECK__
#   define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

int num_succeeded = 0;

void handle_result(bool pass, const char* desc) {
    /* print something reliable, so that if we need to use tools
     * to parse the output of this program to determine support,
     * it will be much easier. */
    if (pass) {
        printf("PASS: %s\n", desc);
        num_succeeded++;
    } else {
        printf("FAIL: %s\n", desc);
    }
}

bool check_sysconf(int val, const char* desc) {
    printf("Checking sysconf(%d) (\"%s\")...\n", val, desc);

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
    printf("Checking system(NULL)...");

    if (0 == system(NULL)) {
        printf("system() is NOT available to execute commands!\n");
        return false;
    } else {
        printf("system() is available to execute commands.\n");
        return true;
    }
}

int main(int argc, char *argv[]) {
#if defined(__STDC_LIB_EXT1_CHECK__) && defined(__STDC_LIB_EXT1__)
    printf("__STDC_LIB_EXT1__ is set!\n");
#endif

    // popen() and pclose()
    bool ret = check_sysconf(_SC_2_VERSION, "popen() and pclose()");
    handle_result(ret, "popen() and pclose()");

    // system()
    ret = check_system();
    handle_result(ret, "system()");

    return num_succeeded > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
