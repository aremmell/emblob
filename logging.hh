#ifndef _MKVEROBJ_LOGGING_HH_INCLUDED
#define _MKVEROBJ_LOGGING_HH_INCLUDED

#include <iostream>
#include <exception>
#include "util.hh"

namespace mkverobj
{
    enum class log_lvl : uint8_t {
        invalid = 0,
        debug,
        info,
        warning,
        error,
        fatal
    };

    static constexpr const char* LOG_LVL_INVALID = "invalid";
    static constexpr const char* LOG_LVL_DEBUG   = "debug";
    static constexpr const char* LOG_LVL_INFO    = "info";
    static constexpr const char* LOG_LVL_WARNING = "warning";
    static constexpr const char* LOG_LVL_ERROR   = "error";
    static constexpr const char* LOG_LVL_FATAL   = "fatal";

    static std::string log_lvl_to_string(log_lvl lvl, bool prefix = false) {
        switch (lvl) {
            case log_lvl::debug:
                return prefix ? "[DEBUG]" : LOG_LVL_DEBUG;
            case log_lvl::info:
                return prefix ? "[INFO]" : LOG_LVL_INFO;
            case log_lvl::warning:
                return prefix ? "[WARNING]" : LOG_LVL_WARNING;
            case log_lvl::error:
                return prefix ? "[ERROR]" : LOG_LVL_ERROR;
            case log_lvl::fatal:
                return prefix ? "[FATAL]" : LOG_LVL_FATAL;
            case log_lvl::invalid:
            default:
                return LOG_LVL_INVALID;
        }
    }

    static log_lvl log_lvl_from_string(const std::string& str) {
        std::string lstr = string_to_lower(str);

        if (lstr == LOG_LVL_DEBUG)
            return log_lvl::debug;
        else if (lstr == LOG_LVL_INFO)
            return log_lvl::info;
        else if (lstr == LOG_LVL_WARNING)
            return log_lvl::warning;
        else if (lstr == LOG_LVL_ERROR)
            return log_lvl::error;
        else if (lstr == LOG_LVL_FATAL)
            return log_lvl::fatal;
        else
            return log_lvl::invalid;
    }

    static void log_msg(log_lvl lvl, const std::string& msg) {

        if (msg.empty() || lvl == log_lvl::invalid)
            return;

        std::ostream& strm = (lvl == log_lvl::error || lvl == log_lvl::fatal) ? std::cerr : std::cout;
        strm << APP_NAME << " " << log_lvl_to_string(lvl, true) << ": " << msg << std::endl;
    }
} // !namespace mkverobj

#endif // !_MKVEROBJ_LOGGING_HH_INCLUDED
