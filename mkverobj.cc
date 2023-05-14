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
#include "cmdline.hh"

using namespace std;
using namespace mkverobj;

int main(int argc, char** argv) {

    int retval = EXIT_FAILURE;
    command_line cmd_line;

    try {
        if (!cmd_line.parse(argc, argv)) {
            command_line::print_usage();
            return retval;
        }   

        // Write binary version data to a temporary file.
        version_resource res;
        res.major = string_to_uint16(argv[1]);
        res.minor = string_to_uint16(argv[2]);
        res.build = string_to_uint16(argv[3]);
        strncpy(res.notes, argv[4], MAX_VER_NOTES - 1);

        auto bin_file = cmd_line.get_bin_output_filename();
        if (!write_binary_version_file(bin_file, res))
            throw new runtime_error(fmt_string("failed to write binary version file %s: (%d) %s",
                                               bin_file.c_str(), errno, strerror(errno)).c_str());

        auto obj_file = cmd_line.get_obj_output_filename();

#if defined(__APPLE__) || defined(__gnu_linux__)

        auto asm_file = cmd_line.get_asm_output_filename();
        ofstream strm(asm_file, ios::out | ios::trunc);
        strm.exceptions(strm.badbit | strm.failbit);

        strm << ".global _version_data" << endl;
        strm << "_version_data:" << endl;
        strm << ".incbin \"" << bin_file << "\"" << endl;
        strm << ".global _sizeof__version_data" << endl;
        strm << ".set _sizeof__version_data, . - _version_data" << endl;

        strm.flush();
            
        if (!file_exists(asm_file))
            throw new runtime_error(fmt_string("failed to write assembly file %s: (%d) %s",
                                               asm_file.c_str(), errno, strerror(errno)).c_str());

        cout << APP_NAME << ": successfully created " << asm_file << endl;

#       pragma message("research correct compiler detection")

        string cc;
        char *env_cc = getenv("CC");
        if (env_cc) {
            cc = env_cc;
            cout << APP_NAME << ": using compiler from CC environment variable: '" << cc << "'" << endl;
        } else {
#           pragma message("pull up compiler selection")

#if defined(__clang__)
            cc = "cc";
#elif defined(__GNUC__)
            cc = "gcc";
#else
#           pragma message("NOTIMPL")
            return EXIT_FAILURE;
#endif
            cerr << APP_NAME << ": WARNING: CC environment variable not set; defaulting to '" << cc << "'" << endl;
        }

        auto cmd = fmt_string("%s -c -o %s %s", cc.c_str(), obj_file.c_str(), asm_file.c_str());
        bool asm_to_obj = execute_shell_command(cmd, true, true);

#       pragma message("TODO: log a warning")        
        delete_file(bin_file);

        retval = asm_to_obj ? EXIT_SUCCESS : EXIT_FAILURE;
#else
#       pragma message("NOTIMPL")
        return EXIT_FAILURE;
#endif
    } catch (exception &ex) {
        cerr << APP_NAME << ": " << ex.what() << endl;
        retval = EXIT_FAILURE;
    }

    // In case an exception was thrown, clean up anything we
    // may have left behind.
    delete_file(cmd_line.get_bin_output_filename());

    return retval;
}
