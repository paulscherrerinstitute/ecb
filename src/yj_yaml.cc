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

#define RYML_SINGLE_HDR_DEFINE_NOW
#include <rapidyaml.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include "yj_yaml.h"
#include "yj_common.h"

using json = nlohmann::json;

void
ecb::YjYaml::read_yaml(const std::string& filename,
    json& json)
{
    std::ifstream yaml_content(filename);

    if (!yaml_content)
        throw std::runtime_error("yaml file not found: " + filename);

    read_yaml(yaml_content, json);
}

void
ecb::YjYaml::read_yaml(std::istream& yaml, json& json)
{
    read_bare_yaml(yaml, json);

    // add ECB metadata
    json::json_pointer meta_ecb_ptr("/meta/ecb");
    json::json_pointer meta_ecb_build_ptr("/meta/ecbBuild");
    json::json_pointer meta_ecb_version_ptr("/meta/ecbVersion");
    json[meta_ecb_ptr] = true;
    json[meta_ecb_build_ptr] = std::stoi(MAKEFILE_BUILD_NUMBER);
    json[meta_ecb_version_ptr] = MAKEFILE_BUILD_VERSION;

    handle_plc_section(json);
    replace_yaml_variables(json);
}

void
ecb::YjYaml::replace_yaml_variables(json& json)
{
    const auto REGEX_find_yaml_variable  = std::regex(R"(\{\{\s*([\w.]+)\s*\}\})");
    const auto REGEX_replace_variable = std::regex(R"((\{\{\s*[\w.]+\s*\}\}))");
    const auto flattend_json = json.flatten();
    nlohmann::json patch;
    std::string replace_value;
    std::string value;
    std::string yaml_variable_key;

    for (const auto& el : flattend_json.items())
    {
        if (el.value().is_string())
        {
            value = el.value();

            for (std::smatch match ; std::regex_search(value, match, REGEX_find_yaml_variable);)
            {
                yaml_variable_key = ecb::yj_common::cfg_key_to_json_key_string(match[1].str());

                if (flattend_json.contains(yaml_variable_key))
                {
                    replace_value = flattend_json[yaml_variable_key];
                    value = std::regex_replace(value, REGEX_replace_variable, replace_value,
                            std::regex_constants::format_first_only);

                    // store new line in patch
                    patch[el.key()] = value;
                }
                else
                    throw std::runtime_error("yaml: unknown variable: " + yaml_variable_key);
            }
        }
    }

    if (patch.empty() == false)
    {
        patch = patch.unflatten();
        json.merge_patch(patch);
    }
}

std::string
ecb::YjYaml::read_yaml_key(std::istream& yaml,
    const std::string& key)
{
    const std::string new_key = ecb::yj_common::cfg_key_to_json_key_string(key);
    json json_data;
    std::string ret_val;
    int temp_int;
    unsigned int temp_uint;
    double temp_double;

    read_bare_yaml(yaml, json_data);
    auto flatten = json_data.flatten();

    if (flatten.contains(new_key))
    {
        auto x = flatten[new_key];

        if (x.is_string())
            ret_val = x;

        if (x.is_boolean())
            (x == true) ? ret_val = "true" : ret_val = "false";

        if (x.is_number_integer())
        {
            temp_int = x;
            ret_val = std::to_string(temp_int);
        }

        if (x.is_number_unsigned())
        {
            temp_uint = x;
            ret_val = std::to_string(temp_uint);
        }

        if (x.is_number_float())
        {
            temp_double = x;
            ret_val = std::to_string(temp_double);
        }
    }

    return ret_val;
}

std::string
ecb::YjYaml::read_yaml_key(const std::string& filename,
    const std::string& key)
{
    std::ifstream yaml_content(filename);

    if (!yaml_content)
        throw std::runtime_error("yaml file not found: " + filename);

    return read_yaml_key(yaml_content, key);
}

std::string
ecb::YjYaml::update_yaml_key(std::istream& yaml, const std::string& key, const std::string& value)
{
    bool is_found = true;
    ryml::Tree tree = ryml::Tree();
    size_t sib_id;
    std::stringstream ret_val;
    std::vector<std::string> split_key;

    std::string yamlContent((std::istreambuf_iterator<char> (yaml)),
        std::istreambuf_iterator<char>());
    ryml::parse_in_place(ryml::to_substr(yamlContent), tree);

    split_key = ecb::yj_common::tokenize(key, ecb::yj_common::REGEX_token_sep_dot);

    sib_id = tree.root_id();

    for (const std::string& el : split_key)
    {
        sib_id = tree.find_child(sib_id, ryml::to_csubstr(el));

        if (sib_id == ryml::NONE)
        {
            is_found = false;
            break;
        }
    }

    if (is_found && tree.has_val(sib_id))
        tree.set_val(sib_id, ryml::to_csubstr(value));

    ret_val << tree;
    return ret_val.str();
}

std::string
ecb::YjYaml::update_yaml_key(
    std::string filename, const std::string& key, const std::string& value)
{
    std::ifstream yaml_content(filename);
    std::string ret_val;

    if (!yaml_content)
        throw std::runtime_error("yaml file not found: " + filename);

    ret_val = update_yaml_key(yaml_content, key, value);
    return ret_val;
}

void
ecb::YjYaml::handle_plc_section(json& json)
{
    auto plc_file_ptr = json::json_pointer("/plc/file");
    std::vector<std::string> plc_code;

    bool is_valid_plc_file = false;

    // if plc.file is a valid file, load it
    if (json.contains(plc_file_ptr))
    {
        std::string filename = json[plc_file_ptr];
        std::filesystem::path plc_file(filename);

        if (std::filesystem::is_regular_file(plc_file))
        {
            is_valid_plc_file = true;
            std::ifstream plc_file_stream(plc_file);

            for (std::string line; std::getline(plc_file_stream, line);)
            {
                yj_common::remove_whitespaces(line);

                if (line.length() != 0)
                    plc_code.push_back(line);
            }
        }
    }

    // if plc.file and plc.code are defined
    auto plc_code_ptr = json::json_pointer("/plc/code");

    if (json.contains(plc_code_ptr) && is_valid_plc_file)
    {
        std::vector<std::string> plc_code_yaml = json[plc_code_ptr];

        for (std::string code_line : plc_code_yaml)
        {
            yj_common::remove_whitespaces(code_line);
            plc_code.push_back(code_line);
        }
    }

    // update plc.code if something changed
    if (plc_code.size() > 0)
    {
        if (is_valid_plc_file || (json.contains(plc_code_ptr) && is_valid_plc_file))
            json[plc_code_ptr] = plc_code;
    }
}

void
ecb::YjYaml::read_bare_yaml(std::istream& yaml, nlohmann::json& json)
{
    std::stringstream json_content;
    std::string yaml_content((std::istreambuf_iterator<char> (yaml)),
        std::istreambuf_iterator<char>());
    ryml::Tree tree;

    ryml::parse_in_place(ryml::to_substr(yaml_content), &tree);
    json_content << ryml::as_json(tree);

    json = json::parse(json_content);
}
