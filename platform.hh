#ifndef _MKVEROBJ_PLATFORM_HH_INCLUDED
#define _MKVEROBJ_PLATFORM_HH_INCLUDED

#if defined(__APPLE__)
# define __MACOS__
# define MKVEROBJ_PLATFORM macOS
#elif defined(__linux__)
# define __LINUS__
# define MKVEROBJ_PLATFORM Linux
#elif defined(_WIN32)
# define MKVEROBJ_PLATFORM Windows
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
# define __BSD__
# define MKVEROBJ_PLATFORM BSD
#else
# error "Unable to determine platform; please contact the author."
#endif

#if !defined(_WIN32)
#if defined(__STDC_WANT_LIB_EXT1__)
# undef __STDC_WANT_LIB_EXT1__
# endif
# define __STDC_WANT_LIB_EXT1__ 1
#else
# if defined(__WANT_STDC_SECURE_LIB__)
# undef __WANT_STDC_SECURE_LIB__
# endif
# define __WANT_STDC_SECURE_LIB__ 1
#endif

#if !defined(restrict)
# if defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
# define restrict __restrict
# elif defined(__GNUC__)
# define restrict __restrict__
#else
# define restrict
# endif
#endif

#if !defined (_WIN32)
# include <sys/wait.h>
# include <unistd.h>
#else
# define WIN32_LEAN_AND_MEAN
# define WINVER 0x0A00
# define _WIN32_WINNT 0x0A00
# include <windows.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <sir.h>
#include <sirfilesystem.h>
#include <siransimacros.h>
#include "util.hh"

#if !defined(_WIN32) && defined(__STDC_LIB_EXT1__)
# define __HAVE_STDC_SECURE_OR_EXT1__
#elif defined(__STDC_SECURE_LIB__)
# define __HAVE_STDC_SECURE_OR_EXT1__
#endif

#if defined(__MACOS__) || defined(__BSD__) || (_POSIX_C_SOURCE >= 200112L && ! _GNU_SOURCE)
# define __HAVE_XSI_STRERROR_R__
#elif defined(_GNU_SOURCE)
# define __HAVE_GNU_STRERROR_R__
#elif defined(__HAVE_STDC_SECURE_OR_EXT1__)
# define __HAVE_STRERROR_S__
# define __HAVE_FOPEN_S__
#endif

#define MAX_ERRORMSG 256

namespace mkverobj
{
    class platform
    {
    public:
        platform() = delete;
        ~platform() = delete;

        static std::string get_error_message(int err) {
            char buf[MAX_ERRORMSG] = {0};

#if defined(__HAVE_XSI_STRERROR_R__)
            bool success = true;
            int finderr = strerror_r(err, buf, MAX_ERRORMSG);

# if defined(__GLIBC__) && (__GLIBC__ >= 2 && __GLIBC_MINOR__ < 13)
            if (finderr == -1) {
                success = false;
                finderr = errno;
            }
# else
            success = finderr == 0;
# endif
            if (!success)
                snprintf(buf, MAX_ERRORMSG, "got error %d while trying to look up error %d", finderr, err);

            return buf;
# elif defined(__HAVE_GNU_STRERROR_R__)
            char* tmp = strerror_r(err, buf, MAX_ERRORMSG);
            return (tmp != buf) ? tmp : buf;
# elif defined(__HAVE_STRERROR_S__)
            [[maybe_unused]] errno_t finderr = strerror_s(buf, MAX_ERRORMSG, err);
            return buf;
# else
            return strerror(err);
#endif
        }

        static bool file_exists(const std::string& fname) {
            bool exists = false;
            if (!_sir_pathexists(fname.c_str(), &exists, SIR_PATH_REL_TO_CWD)) {
                sir_crit("failed to determine existence of '%s'!", fname.c_str());
                return false;
            }

            return exists;
        }

        /* file size in bytes, or -1 upon failure */
        static off_t file_size(const std::string fname) {
            if (fname.empty())
                return -1;

            struct stat st;
            if (!_sir_pathgetstat(fname.c_str(), &st, SIR_PATH_REL_TO_CWD)) {
                sir_crit("failed to stat '%s'!", fname.c_str());
                return -1;
            }

            return st.st_size;
        }

        static bool delete_file(const std::string& fname) {
            return 0 == std::remove(fname.c_str());
        }

        static bool is_valid_output_filename(const std::string& fname, /* [[out]] */ std::string& err_msg) {
            bool created = false;
            err_msg.clear();

            /* std::filesystem, where are you? */
            FILE *f = nullptr;
            int err = 0;

#if defined(__HAVE_STDC_SECURE_LIB__)
            err = fopen_s(&f, fname.c_str(), "wx");
            if (0 == err)
                created = true;
#else
            f = fopen(fname.c_str(), "wx");
            if (f)
                created = true;
            else
                err = errno;
#endif
            if (created) {
                fclose(f);
                f = nullptr;

                if (!delete_file(fname))
                    err_msg = get_error_message(errno);
            } else {
                err_msg = get_error_message(err);
            }

            return created;
        }

        static std::string detect_c_compiler() {
            char* from_env = getenv("CC");
            if (valid_str(from_env)) {
                sir_info("detected C compiler '%s' from environment variable 'CC'", from_env);
                return from_env;
            }

            /* I think the best bet here is to assume that if we run 'cc' it will work
             * 90% of the time (not Windows, though).
             *
             * if i'm able to run system commands, i can run 'which cc' and then 'cc --version'
             * to get some information about the compiler.
             *
             * if i'm not able to run system commands, i am out of ideas for the moment. */

            sir_warn("unable to detect C compiler; defaulting to 'cc'");
            return "cc";
        }

        static bool is_system_command_available() {
            std::cout.flush();

            int ret = std::system(nullptr);
            if (ret == 0) {
                sir_error("system() is NOT available for command processing!");
                return false;
            }

            sir_debug("system() is available for command processing");
            return true;
        }

        static bool execute_system_command(const std::string& cmd) {
            bool retval = false;

            if (is_system_command_available()) {
                std::cout.flush();

                int sysret = std::system(cmd.c_str());

                std::cout.flush();

                int status = WEXITSTATUS(sysret);

                retval = status == 0;

                if (!retval) {
                    sir_error("command '%s' failed (status: %d)", cmd.c_str(), status);
                } else {
                    sir_info("command '%s' succeeded", cmd.c_str());
                }
            }

            return retval;
        }
    };
} // !namespace mkverobj

#endif // ! _MKVEROBJ_PLATFORM_HH_INCLUDED
