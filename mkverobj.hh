#ifndef _MKVEROBJ_HH_INCLUDED
#define _MKVEROBJ_HH_INCLUDED

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "linkers.hh"
#include "version.h"

namespace mkverobj
{
    constexpr static const char* APP_NAME = "mkverobj";
    constexpr static const char* TMP_FILE = "tmp_version";
    
    static inline bool valid_str(const char* str) {
        return nullptr != str && 0 != *str;
    }

    static inline uint16_t string_to_uint16(const char* str) {
        return static_cast<uint16_t>(std::strtoul(str, nullptr, 10));
    }   

    bool validate_command_line(int argc, char** argv) {
        // Count args, make sure they're all valid, blah blah.
        return true;
    }

    int print_usage() {
        std::cerr << "usage:" << std::endl << "\t" << APP_NAME << " <major> <minor> <build> <notes> <output file>" << std::endl;
        return EXIT_FAILURE;
    }

    void write_fake_obj_file(const std::string& filename, const version_resource& res) {
        std::ofstream strm(filename, std::ios::binary | std::ios::trunc);
        strm.exceptions(strm.badbit | strm.failbit);
        strm.write(reinterpret_cast<const char*>(&res), sizeof(res));
        strm.flush();
     }
}; // !namespace mkverobj

#endif // !_MKVEROBJ_HH_INCLUDED
