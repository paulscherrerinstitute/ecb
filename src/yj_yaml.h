//
// ECB - yaml functions
//
// Copyright (C) 2025, Felix Maier <felix.maier@psi.ch>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef _YJ_YAML_H_
#define _YJ_YAML_H_

#include <json.hpp>

namespace ecb
{
class YjYaml
{

public:

    // Reads the YAML content provided in `yaml` or `filename` and stores it in
    // `json`. It calls the `handle_plc_section` and `replace_yaml_variables`
    // functions (refer to its comments for more details).
    void read_yaml(
        std::istream& yaml,
        nlohmann::json& json);

    void read_yaml(
        const std::string& filename,
        nlohmann::json& json);


    // Returns the value of `key` in the YAML content provided in `yaml` or
    // `filename`. If `key` does not exist, the function returns an empty
    // string.
    std::string read_yaml_key(
        std::istream& yaml,
        const std::string& key);

    std::string read_yaml_key(
        const std::string& filename,
        const std::string& key);


    // Updates the value of `key` in the YAML content provided in `yaml` or
    // `filename` with `value`.  It returns the modified YAML content as a
    // string. If the key does not exist, the function returns the unmodified
    // YAML content.
    std::string update_yaml_key(
        std::istream& yaml,
        const std::string& key,
        const std::string& value);

    std::string update_yaml_key(
        std::string filename,
        const std::string& key,
        const std::string& value);

private:


    // Replaces all occurrences of `{{key}}` in the provided `json` with the
    // corresponding value of `key`. If no such placeholders are found, the
    // function returns without modifying `json`. If the key in a placeholder
    // cannot be found this function throws an exception.
    void replace_yaml_variables(nlohmann::json& json);

    // Adds the content of the file defined in `plc.file` to `plc.code`. If
    // `plc.code` and `plc.file` are defined, then the content of `plc.file` is
    // inserted before the content of `plc.code`.
    void handle_plc_section(nlohmann::json& json);

    // Reads `yaml` content and stores it in `json`, no additional processing.
    void read_bare_yaml(
        std::istream& yaml,
        nlohmann::json& json);

};
}

#endif //  _YJ_YAML_H_
