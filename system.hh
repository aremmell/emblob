/*
 * system.hh
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * License:   The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _MKVEROBJ_SYSTEM_HH_INCLUDED
# define _MKVEROBJ_SYSTEM_HH_INCLUDED

#include "platform.hh"
#include "util.hh"

namespace mkverobj
{
    class system
    {
    public:
        system() = delete;
        ~system() = delete;

        static std::string get_error_message(int err) {
            char buf[MAX_ERRORMSG] = {0};

#if defined(__HAVE_XSI_STRERROR_R__)
            bool success = true;
            int finderr = strerror_r(err, buf, MAX_ERRORMSG);
# if defined(__HAVE_XSI_STRERROR_R_ERRNO__)
            if (finderr == -1) {
                success = false;
                finderr = errno;
            }
# else
            success = finderr == 0;
# endif
            if (!success)
                snprintf(buf, MAX_ERRORMSG, "got error %d while trying to look up error %d",
                    finderr, err);

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
            struct stat st;
            int ret = stat(fname.c_str(), &st);
            if (0 != ret) {
                if (ENOENT != errno) {
                    g_logger->error("couldn't stat %s; error: %s", fname.c_str(),
                        get_error_message(errno).c_str());
                }
                return false;
            }

            return true;
        }

        /* file size in bytes, or -1 upon failure */
        static off_t file_size(const std::string fname) {
            struct stat st;
            if (0 != stat(fname.c_str(), &st)) {
                g_logger->error("couldn't stat %s; error: %s", fname.c_str(),
                    get_error_message(errno).c_str());
                return -1;
            }

            return st.st_size;
        }

        static std::ofstream::pos_type write_file_contents(const std::string& fname,
            std::ios_base::openmode mode, const std::function<void(std::ostream&)>& cb) {
            if (!cb)
                return std::ofstream::pos_type(-1);

            try {
                std::ofstream strm(fname, mode);
                strm.exceptions(strm.badbit | strm.failbit);

                cb(strm);
                strm.flush();

                if (strm.good())
                    return strm.tellp();
            } catch (const std::exception& ex) {
                g_logger->error("caught exception while writing to '%s': %s", fname.c_str(),
                    ex.what());
            }

            return std::ofstream::pos_type(-1);
        }

        static bool delete_file(const std::string& fname) {
            return 0 == std::remove(fname.c_str());
        }

        static bool is_valid_output_filename(const std::string& fname, std::string& err_msg) {
            bool created = false;
            err_msg.clear();

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
                g_logger->info("detected C compiler '%s' from environment variable 'CC'",
                    from_env);
                return from_env;
            }

            /* I think the best bet here is to assume that if we run 'cc' it will work
             * 90% of the time (not Windows, though).
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
                std::cout.flush();

                if (!retval) {
                    g_logger->error("command '%s' failed (status: %d)",
                        cmd.c_str(), status);
                } else {
                    g_logger->info("command '%s' succeeded", cmd.c_str());
                }
            }

            return retval;
        }
    };
} // !namespace mkverobj

#endif // !_MKVEROBJ_SYSTEM_HH_INCLUDED
