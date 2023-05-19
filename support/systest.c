#ifdef __STDC_LIB_EXT1_CHECK__
#   define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

long check_sysconf(int val, const char* desc) {
    printf("Checking sysconf(%d) (\"%s\")...\n", val, desc);

    long ret = sysconf(val);
    if (ret == -1) {
        printf("sysconf(%d) error: %s!\n", val, strerror(errno));
    } else {
        printf("sysconf(%d) = %ld.\n", val, ret);
    }

    return ret;
}

int main(int argc, char *argv[]) {
#if defined(__STDC_LIB_EXT1_CHECK__) && defined(__STDC_LIB_EXT1__)
    printf("__STDC_LIB_EXT1__ is set!\n");
#endif

    // popen() and pclose()
    long ret = check_sysconf(_SC_2_VERSION, "poen() and pclose()");

    return ret != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}
