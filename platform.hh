#ifndef _MKVEROBJ_PLATFORM_HH_INCLUDED
#define _MKVEROBJ_PLATFORM_HH_INCLUDED

#define _STR_MACRO(m) #m
#define STR_MACRO(m) _STR_MACRO(m)

#if defined(__APPLE__)
#   define __MACOS__
#   define MKVEROBJ_PLATFORM macOS
#elif defined(__linux__)
#   define __LINUS__
#   define MKVEROBJ_PLATFORM Linux
#elif defined(_WIN32)
#   define MKVEROBJ_PLATFORM Windows
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#   define __BSD__
#   define MKVEROBJ_PLATFORM BSD
#else
#   error "Unable to determine platform; please contact the author."
#endif

#if !defined(_WIN32)
#   define __STDC_WANT_LIB_EXT1__ 1
#else
#   define __WANT_STDC_SECURE_LIB__ 1
#endif

#if !defined (_WIN32)
#   include <sys/wait.h>
#   include <unistd.h>
#else
#   define WIN32_LEAN_AND_MEAN
#   define WINVER 0x0A00
#   define _WIN32_WINNT 0x0A00
#   include <windows.h>
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
#include <string>
#include <cstdint>
#include "logger.hh"

#if !defined(_WIN32) && defined(__STDC_LIB_EXT1__)
#   define __HAVE_STDC_SECURE_OR_EXT1__
#elif defined(__STDC_SECURE_LIB__)
#   define __HAVE_STDC_SECURE_OR_EXT1__
#endif

#if defined(__MACOS__) || defined(__BSD__) || (_POSIX_C_SOURCE >= 200112L && ! _GNU_SOURCE)
#   define __HAVE_XSI_STRERROR_R__
#elif defined(_GNU_SOURCE)
#   define __HAVE_GNU_STRERROR_R__
#elif defined(__HAVE_STDC_SECURE_OR_EXT1__)
#   define __HAVE_STRERROR_S__
#   define __HAVE_FOPEN_S__
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
            char buf[MAX_ERRORMSG] = { 0 };

#if defined(__HAVE_XSI_STRERROR_R__)
            bool success = true;
            int finderr = strerror_r(err, buf, MAX_ERRORMSG);

#if defined(__GLIBC__) && (__GLIBC__ >= 2 && __GLIBC_MINOR__ < 13)
            if (finderr == -1) {
                success = false;
                finderr = errno;
            }
#else
            success = finderr == 0;
#endif
            if (!success)
                snprintf(buf, MAX_ERRORMSG, "got error %d while trying to look up error %d", finderr, err);

            return buf;
#elif defined(__HAVE_GNU_STRERROR_R__)
            char* tmp = strerror_r(err, buf, MAX_ERRORMSG);
            return (tmp != buf) ? tmp : buf;
#elif defined(__HAVE_STRERROR_S__)
            [[maybe_unused]] errno_t finderr = strerror_s(buf, MAX_ERRORMSG, err);
            return buf;
#else
            return strerror(err);
#endif
        }

        static bool file_exists(const std::string& fname) {
#if defined(_WIN32)
            if (TRUE != PathFileExists(fname.c_str()))
                return false;
#else
            struct stat st = {0};
            if (0 != stat(fname.c_str(), &st)) {
                if (errno != ENOENT) {
                    /* questionable; we'd better log this as a warning.
                     * it may be the case that we need to use access() as a backup here. */
                    g_logger->warning("stat(%s) failed, but errno is not ENOENT, it's: '%s' (%d)",
                        fname.c_str(), get_error_message(errno).c_str(), errno);
                }

                return false;
            }
#endif
            return true;
        }

        /* file size in bytes, or -1 upon failure */
        static off_t file_size(const std::string fname) {
            if (fname.empty())
                return -1;

#if defined(_WIN32)

#else
#endif
            return off_t();
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
                g_logger->info("detected C compiler '%s' from environment variable 'CC'", from_env);
                return from_env;
            }

            /* I think the best bet here is to assume that if we run 'cc' it will work
             * 90% of the time (have to check windows).
             * 
             * if i'm able to run system commands, i can run 'which cc' and then 'cc --version'
             * to get some information about the compiler. 
             * 
             * if i'm not able to run system commands, i am out of ideas for the moment. */

            g_logger->warning("unable to detect C compiler; defaulting to 'cc'");
            return "cc";            
        }

        static bool is_system_command_available() {
            std::cout.flush();
            
            int ret = std::system(nullptr);
            if (ret == 0) {
                g_logger->error("system() is NOT available for command processing!");
                return false;
            }

            g_logger->debug("system() is available for command processing");
            return true;
        }

        static bool execute_system_command(const std::string& cmd) {
            bool retval = false;

            if (is_system_command_available()) {
                std::cout.flush();

                int sysret = std::system(cmd.c_str());
                int status = WEXITSTATUS(sysret);

                retval = status == 0;

                if (!retval) {
                    g_logger->error("command '%s' failed (status: %d)", cmd.c_str(), status);
                } else {
                    g_logger->info("command '%s' succeeded", cmd.c_str());
                }            
            }

            return retval;
        }
    };
} // !namespace mkverobj

#endif // ! _MKVEROBJ_PLATFORM_HH_INCLUDED
