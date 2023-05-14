/*
 * @file mkverobj.cc
 * @brief mkverobj API
 *
 * This file and accompanying source code originated from <https://github.com/aremmell/mkverobj/>.
 * If you obtained it elsewhere, all bets are off.
 *
 * @author Ryan M. Lederman <lederman@gmail.com>
 * @copyright
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ryan M. Lederman
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

#include "mkverobj.hh"

using namespace std;
using namespace mkverobj;

int main(int argc, char** argv)
{
    try {
        if (!parse_cmdline(argc, argv))
            return print_usage();

        // Write binary version data to a temporary file.
        version_resource res;
        res.major = string_to_uint16(argv[1]);
        res.minor = string_to_uint16(argv[2]);
        res.build = string_to_uint16(argv[3]);
        strncpy(res.notes, argv[4], MAX_VER_NOTES - 1);

        if (!write_fake_obj_file(TMP_FILE, res))
            throw new runtime_error(fmt_string("failed to write binary version file %s: (%d) %s",
                TMP_FILE, errno, strerror(errno)).c_str());

        // Ensure that the linker object file does not already exist.
        if (filesystem::exists(argv[5])) {
            if (!remove(argv[5]))
                throw new runtime_error(fmt_string("linker object file %s already exists, and can't be deleted: (%d) %s",
                    argv[5], errno, strerror(errno)).c_str());
        }

#if defined(__APPLE__) || defined(__gnu_linux__)

        ofstream strm(INC_FILE, ios::out | ios::trunc);
        strm.exceptions(strm.badbit | strm.failbit);

        strm << ".global _version_data" << endl;
        strm << "_version_data:" << endl;
        strm << ".incbin \"" << TMP_FILE << "\"" << endl;
        strm << ".global _sizeof__version_data" << endl;
        strm << ".set _sizeof__version_data, . - _version_data" << endl;

        strm.flush();

        if (!file_exists(INC_FILE))
            throw new runtime_error(fmt_string("failed to write assembly file %s: (%d) %s",
                INC_FILE, errno, strerror(errno)).c_str());

        cout << APP_NAME << ": successfully created " << INC_FILE << endl;

        // TODO: figure out the right way to get the compiler if it's not in $CC
        string cc;
        char* env_cc = getenv("CC");
        if (env_cc) {
            cc = env_cc;
            cout << APP_NAME << ": using compiler from CC environment variable: '" << cc << "'" << endl;
        } else {
#if defined(__clang__)
            cc = "cc";
#elif defined(__GNUC__)
            cc = "gcc";
#else
#   pragma message("NOTIMPL")
    return EXIT_FAILURE;
#endif
            cerr << APP_NAME << ": WARNING: CC environment variable not set; defaulting to '" << cc << "'" << endl;
        } 

        string cmd = fmt_string("%s -c -o %s %s", cc.c_str(), argv[5], INC_FILE);
        return execute_command(cmd, true, true) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;

/*
.global _version_data

_version_data:
.incbin "VERSION"

.global _sizeof__version_data
.set _sizeof__version_data, . - _version_data
*/
        
#else
    #pragma message("NOTIMPL")
    return EXIT_FAILURE;
#endif

        /*if (0 != remove(TMP_FILE))
            std::cerr << APP_NAME << ": WARNING: unable to remove " << TMP_FILE
                << ": " << strerror(errno) << std::endl;        */
 
        return EXIT_SUCCESS;
    } catch (exception& ex) {
        std::cerr << APP_NAME << ": " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
