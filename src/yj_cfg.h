#ifndef _YJ_CFG_H_
#define _YJ_CFG_H_

#include <string>

namespace ecb
{
class YjConfiguration
{
public:

    // Build the configuration from the given files. The returned string is the
    // rendered configuration for EPICS/ECMC.
    std::string build(
        const std::string& filename_yaml,
        const std::string& filename_schema,
        const std::string& selected_schema,
        const std::string& filename_template,
        const std::string& template_dir);

    // Reads the value of a key from the given YAML file and returns it as a
    // string.  If the key is not defined, an emptry string is returned.
    std::string read_key(
        const std::string& filename_yaml,
        const std::string& key);

    // Updates the value of the specified key in the given YAML file. If the
    // key is not defined, it is not added. In this case the unmodified YAML
    // file is returned.
    std::string update_key(
        const std::string& filename_yaml,
        const std::string& key,
        const std::string& value);
};
}

#endif // _YJ_CFG_H_
