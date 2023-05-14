#ifndef _MKVEROBJ_LOGGING_HH_INCLUDED
#define _MKVEROBJ_LOGGING_HH_INCLUDED

#include <iostream>
#include <exception>
#include "util.hh"

namespace mkverobj
{
    enum class log_lvl : uint8_t
    {
        debug,
        info,
        warning,
        error,
        fatal
    };

    static void log_msg(log_lvl lvl, const std::string& msg) {

        if (msg.empty())
            return;

        std::string prefix;
        std::ostream& strm = (lvl == log_lvl::error || lvl == log_lvl::fatal) ? std::cerr : std::cout;

        switch (lvl) {
            case log_lvl::debug:
                prefix = "[DEBUG]";
                break;
            case log_lvl::info:
                prefix = "[INFO]";
                break;
            case log_lvl::warning:
                prefix = "[WARNING]";
                break;
            case log_lvl::error:
                prefix = "[ERROR]";
                break;
            case log_lvl::fatal:
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
