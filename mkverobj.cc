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
        if (!validate_command_line(argc, argv))
            return print_usage();

        version_resource res;
        res.major = string_to_uint16(argv[1]);
        res.minor = string_to_uint16(argv[2]);
        res.build = string_to_uint16(argv[3]);
        strncpy(res.notes, argv[4], MAX_VER_NOTES - 1);

        write_fake_obj_file(TMP_FILE, res);

        // TODO: don't expect to get LD from the ENV.
        // - it should probably try finding an ld that is native to the
        //   platform, but in cases like cross-compiling, it should be optional
        //   what ld to use, and what commands to send it--otherwise this whole thing is
        //   worthless.
        const char* linker = getenv("LD");
        if (!linker) {
            linker = "ld";
            // TODO: don't really need libsir but this has got to go.
            std::cerr << APP_NAME << ": " << "WARNING: $LD is not set; using 'ld'" << std::endl;
        }

        stringstream cmd;
        stringstream args;

/* #if defined(__gnu_linux__)

        // This one works with GNU ld (ver? distro?).
        // args << " -r -b binary -o VERSION.o tmp_version";
        //

        // OK: when the above doesn't work, this did on:
        // Arch Linux 6.2.14-300.fc38.x86_64 #1 SMP PREEMPT_DYNAMIC x86_64 GNU/Linux
        // and ld v2.39-9.fc38
        //
        // but it only put the contents of the version file into VERSION.o. Something
        // else is still missing:
        //
        // /usr/bin/ld: warning: VERSION.o: missing .note.GNU-stack section implies executable stack
        // /usr/bin/ld: NOTE: This behaviour is deprecated and will be removed in a future version of the linker
        // /usr/bin/ld: build/obj/cxxexample.o: in function `get_version_resource()':
        // cxxexample.cc:(.text+0x7): undefined reference to `_binary_VERSION_start'
        //
        // args << " -Ur --format=binary tmp_version -o VERSION.o";
        //


#elif defined(__APPLE__)

        // -sectcreate is definitely apple-only.
        //args << " -r -o VERSION.o -sectcreate __DATA __version VERSION.o stub.o";
        args << " -v -r -o VERSION.o stub.o";

#else

        // TODO: Write something more clever and helpful.
        #error "Unsupported platform: sorry!"

#endif */

        args << " -Ur --format=binary tmp_version -o VERSION.o";
        cmd << linker << args.str();

        int sysret = system(cmd.str().c_str());

        if (0 != sysret) {
            std::cerr << APP_NAME << ": executing '" << cmd.str() << "' failed: "
                << (-1 == sysret ? -1 : WEXITSTATUS(sysret)) << std::endl;
            return EXIT_FAILURE;
        }

        /*if (0 != remove(TMP_FILE))
            std::cerr << APP_NAME << ": WARNING: unable to remove " << TMP_FILE
                << ": " << strerror(errno) << std::endl;        */
 
        return EXIT_SUCCESS;
    } catch (exception& ex) {
        std::cerr << APP_NAME << ": " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
