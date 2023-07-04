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
#include "appstate.hh"
#include "util.hh"
#include "version.h"

using namespace std;
using namespace mkverobj;

int main(int argc, char** argv) {

    app_state state;
    command_line cmd_line;

    auto _exit_main = [&](int code) {
        if (code != EXIT_SUCCESS) {
            /* If exiting with an error code, clean up any files created;
               don't want to leave things in a half-assed state. */
            if (state.created_bin_file)
                delete_file_on_unclean_exit(cmd_line.get_bin_output_filename());
            if (state.created_asm_file)
                delete_file_on_unclean_exit(cmd_line.get_asm_output_filename());
            if (state.created_obj_file)
                delete_file_on_unclean_exit(cmd_line.get_obj_output_filename());
        }

        sir_debug("exiting with status: %d (%s)", code,
            code == EXIT_SUCCESS ? "success" : "failure");
        sir_cleanup();
        return code;
    };

    try {
        if (!cmd_line.parse_and_validate(argc, argv))
            return _exit_main(cmd_line.print_usage());

        /* Initialize libsir. */
        sirinit si = {0};
        si.d_stdout.opts   = SIRO_NOHOST | SIRO_NOTID;
        si.d_stderr.opts   = si.d_stdout.opts;
        si.d_stdout.levels = SIRL_NONE;
        si.d_stderr.levels = SIRL_NONE;
        si.d_syslog.opts   = SIRO_MSGONLY;
        si.d_syslog.levels = SIRL_NONE;

        sir_level log_level = cmd_line.get_log_level();
        switch (log_level) {
            case SIRL_EMERG:
            case SIRL_ALERT:
            case SIRL_CRIT:
                si.d_stderr.levels = SIRL_CRIT;
            break;
            case SIRL_ERROR:
                si.d_stderr.levels = SIRL_ERROR | SIRL_CRIT;
            break;
            case SIRL_WARN:
                si.d_stderr.levels = SIRL_WARN | SIRL_ERROR | SIRL_CRIT;
            break;
            case SIRL_NOTICE:
            case SIRL_INFO:
                si.d_stderr.levels = SIRL_WARN | SIRL_ERROR | SIRL_CRIT;
                si.d_stdout.levels = SIRL_INFO;
            break;
            case SIRL_DEBUG:
                si.d_stderr.levels = SIRL_WARN | SIRL_ERROR | SIRL_CRIT;
                si.d_stdout.levels = SIRL_DEBUG | SIRL_INFO;
            break;
            case SIRL_NONE:
            case SIRL_ALL:
            case SIRL_DEFAULT:
            default: {
                si.d_stderr.levels = SIRL_WARN | SIRL_ERROR | SIRL_CRIT;
                si.d_stdout.levels = SIRL_INFO;
                sir_warn("invalid logging level; using defaults");
            }
        }

        strncpy(si.name, APP_NAME, SIR_MAXNAME);

        if (!sir_init(&si)) {
            fprintf(stderr, RED("failed to initialize libsir; exiting!") "\n");
            return _exit_main(EXIT_FAILURE);
        }

        std::string compiler = platform::detect_c_compiler();
        if (compiler.empty())
            return _exit_main(EXIT_FAILURE);

        version_resource res;
        res.major = cmd_line.get_major_version();
        res.minor = cmd_line.get_minor_version();
        res.build = cmd_line.get_build_version();

        std::string notes = cmd_line.get_notes();
        if (!notes.empty())
            strncpy(res.notes, notes.c_str(), MKVEROBJ_MAX_NOTES - 1);

        std::string bin_file = cmd_line.get_bin_output_filename();
        sir_info("writing version data {%hu, %hu, %hu, '%s'} to %s...", res.major,
            res.minor, res.build, res.notes, bin_file.c_str());

        auto openmode = ios::out | ios::binary | ios::trunc;
        auto wrote = write_file_contents(bin_file, openmode, [&](ostream& strm) -> void {
            strm.write(reinterpret_cast<const char*>(&res), sizeof(res));
        });

        if (wrote == -1) {
            sir_crit("failed to write %s: %s", bin_file.c_str(),
                platform::get_error_message(errno).c_str());
            return _exit_main(EXIT_FAILURE);
        }

        sir_info("successfully created %s (%lld bytes)", bin_file.c_str(),
            platform::file_size(bin_file));
        state.created_bin_file = true;

#if defined(__MACOS__) || defined(__LINUS__) || defined(__BSD__)
        auto asm_file = cmd_line.get_asm_output_filename();
        openmode = ios::out | ios::trunc;
        wrote = write_file_contents(asm_file, openmode, [&](ostream& strm) -> void {
            strm << ".global _version_data" << endl;
            strm << "_version_data:" << endl;
            strm << ".incbin \"" << bin_file << "\"" << endl;
            strm << ".global _sizeof__version_data" << endl;
            strm << ".set _sizeof__version_data, . - _version_data" << endl;
        });

        if (wrote == -1) {
            sir_crit("failed to write %s: %s", asm_file.c_str(),
                platform::get_error_message(errno).c_str());
            return _exit_main(EXIT_FAILURE);
        }

        sir_info("successfully created %s (%lld bytes)", asm_file.c_str(),
            platform::file_size(asm_file));
        state.created_asm_file = true;

        std::string obj_file = cmd_line.get_obj_output_filename();
        auto cmd = fmt_str("%s -c -o %s %s", compiler.c_str(), obj_file.c_str(), asm_file.c_str());
        bool asm_to_obj = platform::execute_system_command(cmd);

        if (asm_to_obj)
            state.created_obj_file = true;

        return _exit_main(asm_to_obj ? EXIT_SUCCESS : EXIT_FAILURE);
#else
# error "support for this platform/OS has not been implemented; please contact the author."
#endif
    } catch (const exception& ex) {
        sir_crit("caught top-level exception: %s; exiting!", ex.what());
        return _exit_main(EXIT_FAILURE);
    }

    return _exit_main(EXIT_SUCCESS);
}

namespace mkverobj
{
    std::ofstream::pos_type write_file_contents(const std::string& fname,
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
            sir_error("caught exception while writing to '%s': %s", fname.c_str(), ex.what());
        }

        return std::ofstream::pos_type(-1);
    }

    void delete_file_on_unclean_exit(const std::string& fname) {
        if (0 != remove(fname.c_str()))
            sir_error("failed to delete '%s': %s", fname.c_str(),
                platform::get_error_message(errno).c_str());
        else
            sir_info("deleted '%s'", fname.c_str());
    }
} // !namespace mkverobj
