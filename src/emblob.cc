/*
 * emblob.cc
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2024
 * Version:   2.0.0
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
#include "emblob.hh"
#include "emblob/cmdline.hh"
#include "emblob/appstate.hh"
#include "emblob/util.hh"

using namespace std;
using namespace emblob;

int main(int argc, char** argv) {
    app_state state;
    command_line cmd_line;

    auto _exit_main = [&](int code) {
        if (code != EXIT_SUCCESS) {
            /* If exiting with an error code, clean up any files created;
               don't want to leave things in a half-assed state. */
            if (state.created_hdr_file)
                delete_file_on_unclean_exit(cmd_line.get_hdr_output_filename());
            if (state.created_asm_file)
                delete_file_on_unclean_exit(cmd_line.get_asm_output_filename());
            if (state.created_obj_file)
                delete_file_on_unclean_exit(cmd_line.get_obj_output_filename());
        }

        g_logger->debug("exiting with status: %d (%s)", code,
            code == EXIT_SUCCESS ? "success" : "failure");
        return code;
    };

    try {
        int exit_code = EXIT_FAILURE;
        if (!cmd_line.parse_and_validate(argc, argv, exit_code))
            return _exit_main(exit_code);

        auto compiler = system::detect_c_compiler();
        if (compiler.empty())
            return _exit_main(EXIT_FAILURE);

CONST_STATIC_STRING(header_template) = R"EOF(/*
 * emblob_{lname}.h
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2024
 * Version:   2.0.0
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
#ifndef _EMBLOB_{NAME}_H_INCLUDED
#define _EMBLOB_{NAME}_H_INCLUDED

#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>

#if defined(__cplusplus)
# if !defined(EMBLOB_ALIGNAS)
#  define EMBLOB_ALIGNAS alignas
# endif
# if !defined(EMBLOB_EXTERNAL)
#  define EMBLOB_EXTERNAL extern "C"
# endif
#else
# if !defined(EMBLOB_ALIGNAS)
#  if __STDC_VERSION__ < 201710L
#   define EMBLOB_ALIGNAS _Alignas
#  elif __STDC_VERSION__ >= 201710L
#   define EMBLOB_ALIGNAS alignas
#  endif
# endif
# if !defined(EMBLOB_EXTERNAL)
#  define EMBLOB_EXTERNAL extern
# endif
#endif

#if defined(__APPLE__)
# define EMBLOB_{NAME} {lname}_data
#else
# define EMBLOB_{NAME} _{lname}_data
#endif

/**
 * The address of the embedded blob, stored as a pointer-sized unsigned integer.
 */
EMBLOB_EXTERNAL uintptr_t EMBLOB_{NAME};

#if defined(__cplusplus)
    extern "C" {
#endif

/**
 * Returns a pointer to the embedded blob that may be used to access the blob's
 * data one byte (8-bits) at a time.
 */
static inline
const uint8_t* emblob_get_{lname}_8(void)
{
    return (const uint8_t*)&EMBLOB_{NAME};
}

/**
 * Returns a pointer to the embedded blob that may be used to access the blob's
 * data two bytes (16-bits) at a time.
 */
static inline
const uint16_t* emblob_get_{lname}_16(void)
{
    return (const uint16_t*)&EMBLOB_{NAME};
}

/**
 * Returns a pointer to the embedded blob that may be used to access the blob's
 * data four bytes (32-bits) at a time.
 */
static inline
const uint32_t* emblob_get_{lname}_32(void)
{
    return (const uint32_t*)&EMBLOB_{NAME};
}

/**
 * Returns a pointer to the embedded blob that may be used to access the blob's
 * data eight bytes (64-bits) at a time.
 */
static inline
const uint64_t* emblob_get_{lname}_64(void)
{
    return (const uint64_t*)&EMBLOB_{NAME};
}

/**
 * Returns a pointer to the embedded blob that may be used to access the blob's
 * data arbitrarily.
 */
static inline
const void* emblob_get_{lname}_raw(void)
{
    return (const void*)&EMBLOB_{NAME};
}

#if defined(__cplusplus)
    }
#endif

#endif // !_EMBLOB_{NAME}_H_INCLUDED
)EOF";

        auto input_file = cmd_line.get_input_filename();
        auto blob_name = system::file_base_name(input_file);
        auto hdr_file = cmd_line.get_hdr_output_filename();

        std::string header_contents {};
        auto blob_lname = string_to_lower(blob_name);
        auto blob_uname = string_to_upper(blob_name);

        std::regex lexpr("\\{lname\\}");
        header_contents = std::regex_replace(header_template, lexpr, blob_lname);

        std::regex uexpr("\\{NAME\\}");
        header_contents = std::regex_replace(header_contents, uexpr, blob_uname);

        system::delete_file(hdr_file.c_str());

        auto openmode = ios::out | ios::trunc;
        auto wrote = system::write_file_contents(hdr_file, openmode, [&](ostream& strm) -> void {
            strm.write(header_contents.c_str(), header_contents.size());
        });

        if (wrote == -1) {
            g_logger->fatal("failed to write %s: %s", hdr_file.c_str(),
                system::get_error_message(errno).c_str());
            return _exit_main(EXIT_FAILURE);
        }

        g_logger->info("successfully created %s (%lld bytes)", hdr_file.c_str(),
            system::file_size(hdr_file));
        state.created_hdr_file = true;

#if defined(__MACOS__) || defined(__LINUS__) || defined(__BSD__)
        auto asm_file = cmd_line.get_asm_output_filename();
        openmode = ios::out | ios::trunc;
        wrote = system::write_file_contents(asm_file, openmode, [&](ostream& strm) -> void {
            strm << ".global _" << blob_lname << "_data" << endl;
            strm << "_" << blob_lname << "_data:" << endl;
            strm << ".incbin \"" << input_file << "\"" << endl;
            strm << ".global _sizeof__" << blob_lname << "_data" << endl;
            strm << ".set _sizeof__" << blob_lname << "_data, . - _" << blob_lname << "_data" << endl;
        });

        if (wrote == -1) {
            g_logger->fatal("failed to write %s: %s", asm_file.c_str(),
                system::get_error_message(errno).c_str());
            return _exit_main(EXIT_FAILURE);
        }

        g_logger->info("successfully created %s (%lld bytes)", asm_file.c_str(),
            system::file_size(asm_file));
        state.created_asm_file = true;

        auto obj_file = cmd_line.get_obj_output_filename();
        auto cmd = fmt_str("%s -c -o %s %s", compiler.c_str(), obj_file.c_str(), asm_file.c_str());
        bool asm_to_obj = system::execute_system_command(cmd);

        if (asm_to_obj) {
            state.created_obj_file = true;
            g_logger->info("successfully created %s (%lld bytes)", obj_file.c_str(),
            system::file_size(obj_file));
        }

        return _exit_main(asm_to_obj ? EXIT_SUCCESS : EXIT_FAILURE);
#else
# error "support for this platform is not implemented. please contact the author."
#endif
    } catch (const exception& ex) {
        g_logger->fatal("caught top-level exception: %s", ex.what());
        return _exit_main(EXIT_FAILURE);
    }

    return _exit_main(EXIT_SUCCESS);
}

void emblob::delete_file_on_unclean_exit(const std::string& fname) {
    if (0 != remove(fname.c_str()))
        g_logger->error("failed to delete '%s': %s", fname.c_str(),
            system::get_error_message(errno).c_str());
    else
        g_logger->info("deleted '%s'", fname.c_str());
}
