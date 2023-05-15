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
        if (!cmd_line.parse_and_validate(argc, argv)) {
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
        auto openmode = ios::out | ios::binary | ios::trunc;
        auto wrote = write_file_contents(bin_file, openmode, [&](ostream& strm) -> void {
            strm.write(reinterpret_cast<const char*>(&res), sizeof(res));
        });

        if (wrote == -1)
            throw runtime_error(
                fmt_str("failed to write %s: %s", bin_file.c_str(), strerror(errno)).c_str()
            );

        log_msg(log_lvl::info, fmt_str("successfully created %s (%lu bytes)",
            bin_file.c_str(), static_cast<size_t>(wrote)));

#if defined(__APPLE__) || defined(__gnu_linux__)

        auto asm_file = cmd_line.get_asm_output_filename();
        openmode = ios::out | ios::trunc;
        wrote = write_file_contents(asm_file, openmode, [&](ostream& strm) -> void {
            strm << ".global _version_data" << endl;
            strm << "_version_data:" << endl;
            strm << ".incbin \"" << bin_file << "\"" << endl;
            strm << ".global _sizeof__version_data" << endl;
            strm << ".set _sizeof__version_data, . - _version_data" << endl;
        });

        if (wrote == -1)
            throw runtime_error(
                fmt_str("failed to write %s: %s", asm_file.c_str(), strerror(errno)).c_str()
            );

        log_msg(log_lvl::info, fmt_str("successfully created %s (%lu bytes)",
            asm_file.c_str(), static_cast<size_t>(wrote)));

#       pragma message("research correct compiler detection")

        string cc;
        char *env_cc = getenv("CC");
        if (env_cc) {
            cc = env_cc;
            log_msg(log_lvl::info, fmt_str("using compiler from CC environment variable: '%s'", cc.c_str()));
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
            log_msg(log_lvl::warning, fmt_str("CC environment variable not set; defaulting to '%s'", cc.c_str()));
        }

        auto obj_file = cmd_line.get_obj_output_filename();
        auto cmd = fmt_str("%s -c -o %s %s", cc.c_str(), obj_file.c_str(), asm_file.c_str());
        bool asm_to_obj = execute_shell_command(cmd, true, true);

        retval = asm_to_obj ? EXIT_SUCCESS : EXIT_FAILURE;
#else
#       pragma message("NOTIMPL")
        return EXIT_FAILURE;
#endif
    } catch (exception& ex) {
        log_msg(log_lvl::fatal, fmt_str("top-level exception caught: %s", ex.what()));
        retval = EXIT_FAILURE;
    }

    log_msg(log_lvl::debug, fmt_str("exiting with status %d", retval));
    return retval;
}
