#ifndef _MKVEROBJ_LOGGING_HH_INCLUDED
#define _MKVEROBJ_LOGGING_HH_INCLUDED

#include <iostream>
#include <exception>
#include "util.hh"

namespace mkverobj
{
    enum class log_level : uint8_t
    {
        debug,
        info,
        warning,
        error,
        fatal
    };

    static void log_msg(log_level lvl, const std::string& msg) {

        if (msg.empty())
            return;

        std::string prefix;
        std::ostream& strm = (lvl == log_level::error || lvl == log_level::fatal) ? std::cerr : std::cout;

        switch (lvl) {
            case log_level::debug:
                prefix = "[DEBUG]";
                break;
            case log_level::info:
                prefix = "[INFO]";
                break;
            case log_level::warning:
                prefix = "[WARNING]";
                break;
            case log_level::error:
                prefix = "[ERROR]";
                break;
            case log_level::fatal:
                prefix = "[FATAL]";
                break;
            default:
                throw std::runtime_error("invalid log level");
                break;
        }

        strm << APP_NAME << " " << prefix << ": " << msg << std::endl;
    }
} // !namespace mkverobj

#endif // !_MKVEROBJ_LOGGING_HH_INCLUDED
