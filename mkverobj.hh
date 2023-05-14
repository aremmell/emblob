#ifndef _MKVEROBJ_HH_INCLUDED
#define _MKVEROBJ_HH_INCLUDED

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "util.hh"
#include "logging.hh"
#include "version.h"

namespace mkverobj
{   
    static int execute_shell_command(const std::string& cmd, bool echo_stderr = true, bool echo_success = false) {

        int retval = system(cmd.c_str());
        if (0 != retval && echo_stderr) {
            std::cerr << APP_NAME << ": command '" << cmd << "' failed (" << retval << ")" << std::endl;
        } else if (0 == retval && echo_success) {
            std::cout << APP_NAME << ": command '" << cmd << "' succeeded" << std::endl;
        }

        return retval;
    }

    static bool write_binary_version_file(const std::string& filename, const version_resource& res) {
        std::ofstream strm(filename, std::ios::binary | std::ios::trunc);
        strm.exceptions(strm.badbit | strm.failbit);
        strm.write(reinterpret_cast<const char*>(&res), sizeof(res));
        strm.flush();

        return file_exists(filename);
     }
    
} // !namespace mkverobj

#endif // !_MKVEROBJ_HH_INCLUDED
