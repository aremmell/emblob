#ifndef _MKVEROBJ_UTIL_HH_INCLUDED
#define _MKVEROBJ_UTIL_HH_INCLUDED

#include "platform.hh"
#include <cstdint>
#include <string>
#include <fstream>
#include <cstdio>
#include <stdarg.h>
#include <stdlib.h>

namespace mkverobj
{
#define CONST_STATIC_STRING static constexpr const char*

    CONST_STATIC_STRING APP_NAME = "mkverobj";

    static std::string fmt_str(const char* fmt, ...) {
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

    std::string string_to_lower(const std::string& str) {
        std::string retval = str;

        std::for_each(retval.begin(), retval.end(), [](char& c) {
            c = std::tolower(c);
        });

        return retval;
    }

    static bool file_exists(const std::string& filename) {
        std::ifstream strm(filename);
        return strm.good();
    }

    static bool delete_file(const std::string& filename) {
        return 0 == std::remove(filename.c_str());
    }

    static inline bool valid_str(const char* str) {
        return nullptr != str && 0 != *str;
    }

    static inline uint16_t string_to_uint16(const char* str) {
        return static_cast<uint16_t>(std::strtoul(str, nullptr, 10));
    }    

} // !namespace mkverobj

#endif // !_MKVEROBJ_UTIL_HH_INCLUDED
