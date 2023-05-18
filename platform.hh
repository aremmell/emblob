#ifndef _MKVEROBJ_PLATFORM_HH_INCLUDED
#define _MKVEROBJ_PLATFORM_HH_INCLUDED

#define _STR_MACRO(m) #m
#define STR_MACRO(m) _STR_MACRO(m)

#if defined(__APPLE__)
#   define __MACOS__
#   define MKVEROBJ_PLATFORM macOS
#elif defined(__linux__)
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

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>

#if !defined(_WIN32) && defined(__STDC_LIB_EXT1__)
#   define __HAVE_STDC_SECURE_OR_EXT1__
#elif defined(__STDC_SECURE_LIB__)
#   define __HAVE_STDC_SECURE_OR_EXT1__
#endif

#if defined(__MACOS__) || defined(__BSD__) || ((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE)
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
                snprintf(buf, MAX_ERRORMSG, "Got error %d while trying to look up error %d", finderr, err);

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

        static bool is_valid_output_filename(const std::string& fname, /* [[out]] */ std::string& err_msg) {
            bool created = false;
            err_msg.clear();

            /* std::filesystem is not exactly ubiquitous, so we can't rely on it here (yes, in the year 2023). */
            FILE *f = nullptr;
            errno_t err = 0;
            
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

                if (0 != remove(fname.c_str()))
                    err_msg = get_error_message(errno);
            } else {
                err_msg = get_error_message(err);
            }

            return created;
        }
    };
} // !namespace mkverobj

#endif // ! _MKVEROBJ_PLATFORM_HH_INCLUDED
