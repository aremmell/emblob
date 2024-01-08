/*
 * logger.hh
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
#ifndef _EMBLOB_LOGGER_HH_INCLUDED
# define _EMBLOB_LOGGER_HH_INCLUDED

# include "emblob/util.hh"

namespace emblob
{
    class logger : public std::enable_shared_from_this<logger>
    {
    public:
        enum class level {
            invalid = 0,
            debug,
            info,
            warning,
            error,
            fatal
        };

        CONST_STATIC_STRING LEVEL_INVALID = "invalid";
        CONST_STATIC_STRING LEVEL_DEBUG   = "debug";
        CONST_STATIC_STRING LEVEL_INFO    = "info";
        CONST_STATIC_STRING LEVEL_WARNING = "warning";
        CONST_STATIC_STRING LEVEL_ERROR   = "error";
        CONST_STATIC_STRING LEVEL_FATAL   = "fatal";

        static const size_t MAX_LOG_MESSAGE = 2048;

        logger() = default;
        ~logger() = default;

        void debug(const char* fmt, ...) const {
            va_list args;
            va_start(args, fmt);
            _logv(level::debug, fmt, args);
            va_end(args);
        }

        void info(const char* fmt, ...) const {
            va_list args;
            va_start(args, fmt);
            _logv(level::info, fmt, args);
            va_end(args);
        }

        void warning(const char* fmt, ...) const {
            va_list args;
            va_start(args, fmt);
            _logv(level::warning, fmt, args);
            va_end(args);
        }

        void error(const char* fmt, ...) const {
            va_list args;
            va_start(args, fmt);
            _logv(level::error, fmt, args);
            va_end(args);
        }

        void fatal(const char* fmt, ...) const {
            va_list args;
            va_start(args, fmt);
            _logv(level::fatal, fmt, args);
            va_end(args);
        }

        level get_log_level() const {
            return _level;
        }

        void set_log_level(level lvl) {
            if (lvl != level::invalid)
                _level = lvl;
        }

        static std::string level_to_string(level lvl, bool prefix = false) {
            switch (lvl) {
                case level::debug:
                    return prefix ? "[debug]" : LEVEL_DEBUG;
                case level::info:
                    return prefix ? "[info]" : LEVEL_INFO;
                case level::warning:
                    return prefix ? "[warning]" : LEVEL_WARNING;
                case level::error:
                    return prefix ? "[error]" : LEVEL_ERROR;
                case level::fatal:
                    return prefix ? "[fatal]" : LEVEL_FATAL;
                case level::invalid:
                default:
                    return LEVEL_INVALID;
            }
        }

        static level level_from_string(const std::string& str) {
            std::string lstr = string_to_lower(str);
            if (lstr == LEVEL_DEBUG)
                return level::debug;
            else if (lstr == LEVEL_INFO)
                return level::info;
            else if (lstr == LEVEL_WARNING)
                return level::warning;
            else if (lstr == LEVEL_ERROR)
                return level::error;
            else if (lstr == LEVEL_FATAL)
                return level::fatal;
            else
                return level::invalid;
        }

    private:
        void _logv(level lvl, const char* fmt, va_list args) const {
            if (!valid_str(fmt) || lvl == level::invalid)
                return;

            if (lvl < _level)
                return;

            char buf[MAX_LOG_MESSAGE] = {0};
            [[maybe_unused]] int len = std::vsnprintf(buf, MAX_LOG_MESSAGE, fmt, args);


            const char* attr     = "0";
            const char* fg_color = "39";
            switch (lvl) {
                case level::debug:
                    fg_color = "90";
                    break;
                case level::info:
                    fg_color = "97";
                    break;
                case level::warning:
                    fg_color = "33";
                    break;
                case level::error:
                    fg_color = "91";
                    break;
                case level::fatal:
                    attr     = "1";
                    fg_color = "91";
                    break;
                case level::invalid:
                default:
                    return;
            }

            std::ostream& strm = (lvl == level::error || lvl == level::fatal) ? std::cerr : std::cout;
            strm << "\x1b[" << attr << ";" << fg_color << ";49m" << APP_NAME << " "
                << level_to_string(lvl, true) << ": " << buf << "\x1b[0m" << std::endl;
        }

    private:
# if defined(DEBUG)
        level _level = level::debug;
# else
        level _level = level::info;
# endif
    };

    typedef std::unique_ptr<logger> logger_ptr;
    logger_ptr g_logger = std::make_unique<logger>();

} // !namespace emblob

#endif // !_EMBLOB_LOGGER_HH_INCLUDED
