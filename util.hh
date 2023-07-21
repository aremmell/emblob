/*
 * util.hh
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
#ifndef _MKVEROBJ_UTIL_HH_INCLUDED
#define _MKVEROBJ_UTIL_HH_INCLUDED

#include <string>
#include <fstream>
#include <cstdio>
#include <cstdarg>
#include <algorithm>

namespace mkverobj
{
#define CONST_STATIC_X(type) static constexpr type
#define CONST_STATIC_STRING CONST_STATIC_X(const char*)

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

    static inline bool valid_str(const char* str) {
        return nullptr != str && 0 != *str;
    }

    static inline uint16_t string_to_uint16(const std::string& str) {
        return static_cast<uint16_t>(std::strtoul(str.c_str(), nullptr, 10));
    }

} // !namespace mkverobj

#endif // !_MKVEROBJ_UTIL_HH_INCLUDED
