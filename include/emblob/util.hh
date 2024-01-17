/*
 * util.hh
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
#ifndef _EMBLOB_UTIL_HH_INCLUDED
# define _EMBLOB_UTIL_HH_INCLUDED

# include "platform.hh"

namespace emblob
{
# define CONST_STATIC_X(type) static constexpr type
# define CONST_STATIC_STRING CONST_STATIC_X(const char*)

    CONST_STATIC_STRING APP_NAME = "emblob";

    static std::string fmt_str(const char* fmt, ...) {
        va_list args1;
        va_start(args1, fmt);
        va_list args2;
        va_copy(args2, args1);

        int len = std::vsnprintf(nullptr, 0, fmt, args1);
        va_end(args1);

        if (len < 0) {
            return std::string();
        }

        auto buf = std::make_unique<char[]>(len + 1);
        std::vsnprintf(buf.get(), len + 1, fmt, args2);
        va_end(args2);

        if (len < 0) {
            return std::string();
        }

        return buf.get();
    }

    std::string string_to_lower(const std::string& str) {
        auto retval = str;

        std::ranges::for_each(retval.begin(), retval.end(), [](char& c) {
            c = static_cast<char>(std::tolower(c));
        });

        return retval;
    }

    std::string string_to_upper(const std::string& str) {
        auto retval = str;

        std::ranges::for_each(retval.begin(), retval.end(), [](char& c) {
            c = static_cast<char>(std::toupper(c));
        });

        return retval;
    }

    static inline bool valid_str(const char* str) {
        return nullptr != str && 0 != *str;
    }

} // !namespace emblob

#endif // !_EMBLOB_UTIL_HH_INCLUDED
