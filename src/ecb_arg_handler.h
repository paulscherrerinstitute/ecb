#ifndef _ECB_ARG_HANDLER_H_
#define _ECB_ARG_HANDLER_H_

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace ecb
{
enum class mode
{
    INVALID,
    BUILD_INFO,
    HELP,
    YJ_BUILD_CFG_TO_FILE,
    YJ_BUILD_CFG_TO_STDOUT,
    YJ_READ_KEY_TO_FILE,
    YJ_READ_KEY_TO_STDOUT,
    YJ_UPDATE_KEY,
    YJ_UPDATE_KEY_TO_STDOUT,
};


class ArgHandler
{

public:
    // Adds a command line argument with a specified value to the state
    // of this module. The mode (refer to `get_mode()`) is determined
    // based on the provided command line arguments.  Returns true if
    // the command line argument and value are valid.  Returns false if
    // an unknown command line argument or value is set.
    bool set_argument(
        std::string_view arg,
        std::string_view value);


    // Returns the mode based on the defined command line arguments.  If no
    // valid argument combination is provided, `ecb::mode:INVALID` is
    // returned. Call this function after setting all arguments.
    ecb::mode get_mode(void);


    // Returns the value set via the command line parameter `--yaml`, which is
    // the filename of the YAML configuration. If `--yaml` is not provided,
    // this function returns an empty string.
    std::string get_yj_yaml_filename(void);


    // Returns the filename of the output file provided via the command line
    // parameter `--output`. If `--output` is not provided, this functions
    // returns an empty string.
    std::string get_output_filename(void);


    // Returns the filename of the specified Jinja template file, set by the
    // command line argument `--template`. If `--template` is not provided,
    // this function returns an empty string.
    std::string get_yj_template_filename(void);


    // Returns the name of the key specified by the command line argument
    // `--key`. If `--key` is not provided, this function returns an empty
    // string.
    std::string get_yj_key_value(void);


    // Returns the schema specified by the command line argument `--schema`.
    // This is the schema to be used, e.g. "axis" or "plc". If `--schema` is
    // not provided, this function returns an empty string.
    std::string get_yj_schema(void);


    // Returns the filename of the specified schema JSON file, as given by the
    // command line argument `--schemafile`. If `--schemafile` is not provided,
    // this function returns an empty string.
    std::string get_yj_schema_filename(void);


    // Returns the path to the specified template directory, as given by the
    // command line argument `--templatedir`. If `--templatedir` is not
    // provided, this function returns an empty string.
    std::string get_yj_template_dir(void);


    // Returns the value specified by the  command line parameter  "--value".
    // If `--value` is not provided, this function returns an empty string.
    std::string get_yj_value(void);

private:
    std::unordered_map<std::string, std::string> args_;

};
}

#endif // _ECB_ARG_HANDLER_H_
