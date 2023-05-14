#ifndef _MKVEROBJ_HH_INCLUDED
#define _MKVEROBJ_HH_INCLUDED

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include "util.hh"
#include "logging.hh"
#include "version.h"

namespace mkverobj
{   
    static bool execute_shell_command(const std::string& cmd, bool echo_stderr = true, bool echo_success = false) {
        bool retval = false;

        std::cout.flush();

        int sysret = std::system(cmd.c_str());
        int status = WEXITSTATUS(sysret);

        retval = status == 0;

        if (!retval && echo_stderr) {
            log_msg(log_lvl::error, fmt_str("command '%s' failed (status: %d)", cmd.c_str(), status));
        } else if (retval && echo_success) {
            log_msg(log_lvl::info, fmt_str("command '%s' succeeded", cmd.c_str()));
        }

        return retval;
    }

    static std::ofstream::pos_type write_file_contents(const std::string& filename, std::ios_base::openmode mode, const std::function<void(std::ostream&)>& cb)
    {
        if (!cb)
            return std::ofstream::pos_type(-1);

        std::ofstream strm(filename, mode);
        strm.exceptions(strm.badbit | strm.failbit);

        cb(strm);
        strm.flush();

        if (strm.good() && file_exists(filename)) {
            return strm.tellp();
        }

         return std::ofstream::pos_type(-1);
    }
    
} // !namespace mkverobj

#endif // !_MKVEROBJ_HH_INCLUDED
