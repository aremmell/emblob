#ifndef _MKVEROBJ_CMDLINE_HH_INCLUDED
#define _MKVEROBJ_CMDLINE_HH_INCLUDED

#include <cstring>
#include <iostream>
#include <functional>
#include <vector>
#include <regex>
#include <limits>
#include "util.hh"
#include "logging.hh"

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
            /*struct config
            {
                struct arg_value
                {
                    std::string value;
                };

                struct arg
                {
                    int index = 0;
                    std::string flag;
                    std::string short_flag;
                    std::string description;
                    bool required = false;
                    int expected_values = 1;           
                    std::vector<arg_value> values;
                    std::function<bool(const arg_value& value, [[out]] std::string& msg)> validator;
                };
            };*/

            static bool _version_number_validator(const std::string& val, [[out]] std::string& msg) {

                msg.clear();

                if (val.empty()) {
                    msg = "is required.";
                    return false;
                }

                try {
                    // all digits, not greater than 65535
                    auto max_value = static_cast<unsigned long>(std::numeric_limits<uint16_t>::max());
                    std::basic_regex expr("/^[0-9]{1,5}$/");
                    if (!std::regex_match(val, expr) || std::strtoul(val.c_str(), nullptr, 10) > max_value) {
                        msg = "must be a number between 0 and 65535.";
                        return false;
                    }
                } catch (std::regex_error& e) {
                    log_msg(log_lvl::error, fmt_str("regex error: %s", e.what()));
                    return false;
                }

                return true;
            };                      

            //config _config;
            std::string _output_filename;
    };
} // !namespace mkverobj

#endif // !_MKVEROBJ_CMDLINE_HH_INCLUDED
