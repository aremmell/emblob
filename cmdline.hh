#ifndef _MKVEROBJ_CMDLINE_HH_INCLUDED
#define _MKVEROBJ_CMDLINE_HH_INCLUDED

#include <cstring>
#include <iostream>
#include "util.hh"

namespace mkverobj
{
    class command_line
    {
    public:
        constexpr static const char* EXT_BIN = ".bin";
        constexpr static const char* EXT_ASM = ".S";
        constexpr static const char* EXT_OBJ = ".o";

        command_line() = default;

        bool parse_and_validate(int argc, char** argv) {
#           pragma message("TODO: implement command_line::parse_and_validate")
            if (argc < 6)
                return false;

            if (0 == strcmp(argv[5], ""))
                return false;

            set_output_filename(argv[5]);

            return true;
        }

        static void print_usage() {
#           pragma message("TODO: implement command_line::print_usage")        
            std::cerr << fmt_str("usage:\n\t%s <major> <minor> <build> <notes> <output file>", APP_NAME) << std::endl;
        }

        void set_output_filename(const char* filename) {
            _output_filename = filename;
        }

        std::string get_bin_output_filename() const {
            return _output_filename + EXT_BIN;
        }

        std::string get_asm_output_filename() const {
            return _output_filename + EXT_ASM;
        }

        std::string get_obj_output_filename() const {
            return _output_filename + EXT_OBJ;
        }        

        private:
            std::string _output_filename;
    };
} // !namespace mkverobj

#endif // !_MKVEROBJ_CMDLINE_HH_INCLUDED
