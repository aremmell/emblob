#ifndef _MKVEROBJ_CMDLINE_HH_INCLUDED
#define _MKVEROBJ_CMDLINE_HH_INCLUDED

#include <cstring>
#include <iostream>
#include <functional>
#include <vector>
#include <regex>
#include <limits>
#include "util.hh"
#include "logger.hh"

namespace mkverobj
{
    class command_line
    {
    public:
        CONST_STATIC_STRING EXT_BIN = ".bin";
        CONST_STATIC_STRING EXT_ASM = ".S";
        CONST_STATIC_STRING EXT_OBJ = ".o";
        CONST_STATIC_STRING DEF_OUTPUT_FILE = "__version";

        CONST_STATIC_STRING FLAG_MAJOR = "--major";
        CONST_STATIC_STRING S_FLAG_MAJOR = "-maj";

        CONST_STATIC_STRING FLAG_MINOR = "--minor";
        CONST_STATIC_STRING S_FLAG_MINOR = "-min";

        CONST_STATIC_STRING FLAG_BUILD = "--build";
        CONST_STATIC_STRING S_FLAG_BUILD = "-b";

        CONST_STATIC_STRING FLAG_NOTES = "--notes";
        CONST_STATIC_STRING S_FLAG_NOTES = "-n";

        CONST_STATIC_STRING FLAG_OUTPUT_FILE = "--outfile";
        CONST_STATIC_STRING S_FLAG_OUTPUT_FILE = "-o";

        CONST_STATIC_STRING FLAG_LOG_LEVEL = "--log-level";
        CONST_STATIC_STRING S_FLAG_LOG_LEVEL = "-l";

        CONST_STATIC_STRING FLAG_HELP = "--help";
        CONST_STATIC_STRING S_FLAG_HELP = "-h";             

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

                if (a->flag == FLAG_HELP) {
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
            return string_to_uint16(_config.get_value(FLAG_MAJOR).c_str());
        }

        uint16_t get_minor_version() const {
            return string_to_uint16(_config.get_value(FLAG_MINOR).c_str());
        }     

        uint16_t get_build_version() const {
            return string_to_uint16(_config.get_value(FLAG_BUILD).c_str());
        }     

        std::string get_notes() const {
            return _config.get_value(FLAG_NOTES);
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

        logger::level get_logging_level() const {
            return logger::level_from_string(_config.get_value(FLAG_LOG_LEVEL));
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

                        if (!required && flag != FLAG_HELP) {
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
                    { FLAG_MAJOR, S_FLAG_MAJOR, "Major version number", "", "", {}, true, true, false, false, &_version_number_validator },
                    { FLAG_MINOR, S_FLAG_MINOR, "Minor version number", "", "", {}, true, true, false, false, &_version_number_validator },
                    { FLAG_BUILD, S_FLAG_BUILD, "Build number", "", "", {}, false, true, false, false, &_version_number_validator },
                    { FLAG_NOTES, S_FLAG_NOTES, "Notes (max 256 characters)", "", "", {}, false, true, false, false, nullptr },
                    { FLAG_OUTPUT_FILE, S_FLAG_OUTPUT_FILE, "Output file name", "", DEF_OUTPUT_FILE, {}, false, true, false, false, &_output_filename_validator },
                    { FLAG_LOG_LEVEL, S_FLAG_LOG_LEVEL, "Console logging verbosity", "", logger::level_to_string(logger::level::info), {
                        logger::level_to_string(logger::level::debug),
                        logger::level_to_string(logger::level::info),
                        logger::level_to_string(logger::level::warning),
                        logger::level_to_string(logger::level::error),
                        logger::level_to_string(logger::level::fatal),
                    }, false, false, false, false, &_log_level_validator },
                    { FLAG_HELP, S_FLAG_HELP, "Print this usage information", "", "", {}, false, false, false, false, nullptr }
                };
            };

            std::string _get_output_filename(const char *ext) const {
                std::string val = _config.get_value(FLAG_OUTPUT_FILE);
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

                if (logger::level::invalid == logger::level_from_string(val)) {
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
                    std::regex expr("^[0-9]{1,5}$");
                    if (!std::regex_match(val, expr) || std::strtoul(val.c_str(), nullptr, 10) > max_value) {
                        msg = "must be a number between 0 and 65535.";
                        return false;
                    }
                } catch (std::regex_error& e) {
                    g_logger->error("regex exception: %s", e.what());
                    return false;
                }

                return true;
            };                      

            config _config;
            std::string _output_filename;
    };
} // !namespace mkverobj

#endif // !_MKVEROBJ_CMDLINE_HH_INCLUDED
