#ifndef _MKVEROBJ_HH_INCLUDED
#define _MKVEROBJ_HH_INCLUDED

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "linkers.hh"
#include "version.h"

namespace mkverobj
{
    constexpr static const char* APP_NAME = "mkverobj";
    constexpr static const char* TMP_FILE = "VERSION";
    constexpr static const char* INC_FILE = "version.S";
    
    static inline bool valid_str(const char* str) {
        return nullptr != str && 0 != *str;
    }

    static inline uint16_t string_to_uint16(const char* str) {
        return static_cast<uint16_t>(std::strtoul(str, nullptr, 10));
    }

    std::string fmt_string(const char* fmt, ...) {
        va_list args1;
        va_start(args1, fmt);
        va_list args2;
        va_copy(args2, args1);

        char* buf = nullptr;
        int len = std::vsnprintf(buf, 0, fmt, args1);
        va_end(args1);

        if (len < 0)
            return std::string();

        buf = new char[len + 1];
        std::vsnprintf(buf, len + 1, fmt, args2);
        va_end(args2);

        if (len < 0)
            return std::string();

        std::string retval(buf);
        delete[] buf;
        return retval;
    }    

    bool parse_cmdline(int argc, char** argv) {
        if (argc < 6)
            return false;

        return true;
    }

    int print_usage() {
        std::cerr << fmt_string("usage:\n\t%s <major> <minor> <build> <notes> <output file>", APP_NAME) << std::endl;
        return EXIT_FAILURE;
    }

    bool write_fake_obj_file(const std::string& filename, const version_resource& res) {
        std::ofstream strm(filename, std::ios::binary | std::ios::trunc);
        strm.exceptions(strm.badbit | strm.failbit);
        strm.write(reinterpret_cast<const char*>(&res), sizeof(res));
        strm.flush();

        return std::filesystem::exists(filename);
     }

     int execute_command(const std::string& cmd, bool echo_stderr = true, bool echo_success = false) {

        int retval = system(cmd.c_str());
        if (0 != retval && echo_stderr) {
            std::cerr << APP_NAME << ": command '" << cmd << "' failed (" << retval << ")" << std::endl;
        } else if (0 == retval && echo_success) {
            std::cout << APP_NAME << ": command '" << cmd << "' succeeded" << std::endl;
        }

        return retval;
     }

}; // !namespace mkverobj

#endif // !_MKVEROBJ_HH_INCLUDED
