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
        constexpr static const char* DEF_OUTPUT_FILE = "__version";

        static constexpr const char* flag_major = "--major";
        static constexpr const char* s_flag_major = "-maj";

        static constexpr const char* flag_minor = "--minor";
        static constexpr const char* s_flag_minor = "-min";

        static constexpr const char* flag_build = "--build";
        static constexpr const char* s_flag_build = "-b";

        static constexpr const char* flag_notes = "--notes";
        static constexpr const char* s_flag_notes = "-n";

        static constexpr const char* flag_output_file = "--output";
        static constexpr const char* s_flag_output_file = "-o";

        static constexpr const char* flag_log_level = "--log-level";
        static constexpr const char* s_flag_log_level = "-l";

        static constexpr const char* flag_help = "--help";
        static constexpr const char* s_flag_help = "-h";             

        command_line() = default;

        bool parse_and_validate(int argc, char** argv) {
            int required = 0;
            
            _config.for_each([&](const config::arg& a) {
                if (a.required)
                    required++;
                if (a.value_required && a.default_value.empty())
                    required++;
            });

            for (int i = 1; i < argc; i++) {
                std::string input = argv[i];
                
                config::arg *a = nullptr;
                if (!_config.get_arg(input, &a)) {
                    std::cerr << "Unknown option: " << input << std::endl;
                    return false;
                }

                if (a->flag == flag_help) {
                    [[maybe_unused]] int p = print_usage();
                    return true;
                }

                a->seen = true;

                if (a->value_required) {
                    if (i + 1 > argc - 1) {
                        std::cerr << "Missing value for " << input << std::endl;
                        return false;
                    }

                    std::string validate_msg;
                    if (a->validator != nullptr && !a->validator(argv[i + 1], validate_msg)) {
                        std::cerr << argv[i + 1] << " is not a valid value for " << input << " (" << validate_msg << ")" << std::endl;
                        return false;
                    }
            
                    a->value = argv[i++ + 1];
                    a->validated = true;
                }
            }

            return true;
        }

        int print_usage() const {
            std::cerr << APP_NAME << " usage:" << std:: endl;
            _config.for_each([](const config::arg& a) {
                std::cerr << "\t" << a.to_string() << std::endl;
            });

            return EXIT_FAILURE;
        }

        uint16_t get_major_version() const {
            return string_to_uint16(_config.get_value(flag_major).c_str());
        }

        uint16_t get_minor_version() const {
            return string_to_uint16(_config.get_value(flag_minor).c_str());
        }     

        uint16_t get_build_version() const {
            return string_to_uint16(_config.get_value(flag_build).c_str());
        }     

        std::string get_notes() const {
            return _config.get_value(flag_notes);
        }

        std::string get_bin_output_filename() const {
            return _get_output_filename(EXT_BIN);
        }

        std::string get_asm_output_filename() const {
            return _get_output_filename(EXT_ASM);
        }

        std::string get_obj_output_filename() const {
            return _get_output_filename(EXT_OBJ);
        }        

        log_lvl get_logging_level() const {
            return log_lvl_from_string(_config.get_value(flag_log_level));
        }

        private:
            struct config
            {
                struct arg
                {
                    /* currently only supporting one parameter per flag */
                    std::string flag;
                    std::string short_flag;
                    std::string description;
                    std::string value;
                    std::string default_value;
                    std::vector<std::string> options;
                    bool required = false;
                    bool value_required = false;
                    bool seen = false;
                    bool validated = false;
                    std::function<bool(const std::string& value, /* [out] */ std::string& msg)> validator;

                    std::string options_to_string() const {
                        if (options.empty())
                            return std::string();

                        std::string retval = "{";
                        bool first = true;

                        for (const auto& o : options) {
                            if (first) {
                                retval += o;
                                first = false;
                            } else {
                                retval += " | " + o;
                            }
                        }

                        return retval + "}";
                    }

                    std::string to_string() const {
                        std::string retval; 
                        
                        retval += flag + ", " + short_flag + "\t" + description;                            

                        std::string options_str = options_to_string();
                        if (!options_str.empty())
                            retval += " : " + options_str;;

                        if (!required && flag != flag_help) {
                            retval += " (optional";

                            if (!default_value.empty())
                                retval += "; default: '" + default_value + "'";

                            retval += ")";
                        }

                        return retval;
                    }

                    std::string to_state_string() const {
                        return fmt_str("value: '%s', default value: '%s', seen: %s, validated: %s",
                            value.c_str(), default_value.c_str(), seen ? "true" : "false", validated ? "true" : "false");
                    }
                };

                void for_each(const std::function<void(const arg& a)>& func) const {
                    if (!func)
                        return;
                    
                    for (const auto& a : args)
                        func(a);
                }

                bool get_arg(const std::string& flag, /* [[out]] */ arg** out) {
                    if (!out)
                        return false;

                    for (auto& a : args) {
                        if (a.flag == flag || a.short_flag == flag) {
                            *out = &a;
                            return true;
                        }
                    }

                    return false;
                }

                std::string get_value(const std::string& flag) const {
                    for (const auto& a : args) {
                        if (a.flag == flag || a.short_flag == flag) {
                            if (a.value.empty() && !a.default_value.empty())
                                return a.default_value;
                            else
                                return a.value;
                        }
                    }

                    return std::string();
                }                                                                                           

                std::vector<arg> args = {
                    { flag_major, s_flag_major, "Major version number", "", "", {}, true, true, false, false, &_version_number_validator },
                    { flag_minor, s_flag_minor, "Minor version number", "", "", {}, true, true, false, false, &_version_number_validator },
                    { flag_build, s_flag_build, "Build number", "", "", {}, false, true, false, false, &_version_number_validator },
                    { flag_notes, s_flag_notes, "Notes (max 256 characters)", "", "", {}, false, true, false, false, nullptr },
                    { flag_output_file, s_flag_output_file, "Output file name", "", DEF_OUTPUT_FILE, {}, false, false, false, false, &_output_filename_validator },
                    { flag_log_level, s_flag_log_level, "Console logging verbosity", "", log_lvl_to_string(log_lvl::info), {
                        log_lvl_to_string(log_lvl::debug),
                        log_lvl_to_string(log_lvl::info),
                        log_lvl_to_string(log_lvl::warning),
                        log_lvl_to_string(log_lvl::error),
                        log_lvl_to_string(log_lvl::fatal),
                    }, false, false, false, false, &_log_level_validator },
                    { flag_help, s_flag_help, "Print this usage information", "", "", {}, false, false, false, false, nullptr }
                };
            };

            std::string _get_output_filename(const char *ext) const {
                std::string val = _config.get_value(flag_output_file);
                if (!val.empty())
                    return val + ext;
                else
                    return std::string();
            }

            static bool _log_level_validator(const std::string& val, /* [[out]] */ std::string& msg) {

                msg.clear();

                if (val.empty()) {
                    msg = "no level specified.";
                    return false;
                }

                if (log_lvl::invalid == log_lvl_from_string(val)) {
                    msg = fmt_str("%s is not a valid log level.", val.c_str());
                    return false;
                }

                return true;
            }

            static bool _output_filename_validator(const std::string& val, /* [[out]] */ std::string& msg) {

                msg.clear();

                if (val.empty()) {
                    msg = "no filename specified.";
                    return false;
                }

                for (const auto& ext : { EXT_BIN, EXT_ASM, EXT_OBJ }) {
                    std::string file_err_msg;
                    if(!platform::is_valid_output_filename(val + ext, file_err_msg)) {
                        msg = fmt_str("Unable to use %s as an output filename (%s)", val.c_str(), file_err_msg.c_str());
                        return false;
                    }
                }

                return true;
            }

            static bool _version_number_validator(const std::string& val, /* [[out]] */ std::string& msg) {

                msg.clear();

                if (val.empty()) {
                    msg = "is required.";
                    return false;
                }

                try {
                    // all digits, not greater than 65535
                    auto max_value = static_cast<unsigned long>(std::numeric_limits<uint16_t>::max());
                    std::basic_regex expr("^[0-9]{1,5}$");
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

            config _config;
            std::string _output_filename;
    };
} // !namespace mkverobj

#endif // !_MKVEROBJ_CMDLINE_HH_INCLUDED
