#ifndef _MKVEROBJ_LOGGER_HH_INCLUDED
#define _MKVEROBJ_LOGGER_HH_INCLUDED

#include <iostream>
#include <exception>
#include <memory>
#include "util.hh"

namespace mkverobj
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
                    return prefix ? "[DEBUG]" : LEVEL_DEBUG;
                case level::info:
                    return prefix ? "[INFO]" : LEVEL_INFO;
                case level::warning:
                    return prefix ? "[WARNING]" : LEVEL_WARNING;
                case level::error:
                    return prefix ? "[ERROR]" : LEVEL_ERROR;
                case level::fatal:
                    return prefix ? "[FATAL]" : LEVEL_FATAL;
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
            int len = std::vsnprintf(buf, MAX_LOG_MESSAGE, fmt, args);

            std::ostream& strm = (lvl == level::error || lvl == level::fatal) ? std::cerr : std::cout;
            strm << APP_NAME << " " << level_to_string(lvl, true) << ": " << buf << std::endl;
        }        

    private:
#if defined(DEBUG)
        level _level = level::debug;
#else    
        level _level = level::info;
#endif
    };

    typedef std::unique_ptr<logger> logger_ptr;
    logger_ptr g_logger = std::make_unique<logger>();

} // !namespace mkverobj

#endif // !_MKVEROBJ_LOGGER_HH_INCLUDED
