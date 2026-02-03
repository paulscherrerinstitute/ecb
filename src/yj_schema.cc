//
// ECB - module to validate yaml configuration
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

#include <fstream>
#include <iostream>
#include <regex>

#include "yj_common.h"
#include "yj_schema.h"

using nlohmann::json;

const std::regex REGEX_norm_pair = std::regex(R"(\s*(.+?)=(.+))");


ecb::YjSchema::YjSchema(std::string filename_schema, const std::string& selected_schema)
{
    std::ifstream ifs(filename_schema);

    if (!ifs)
        throw std::runtime_error("schema file not found: " + filename_schema);

    schema_ = nlohmann::json::parse(ifs);
    schema_ = schema_.flatten();
    grand_schema_ = selected_schema;
    is_schemas_fetched_ = false;
}

ecb::YjSchema::YjSchema(std::istream& schema, const std::string& selected_schema)
{
    schema_ = nlohmann::json::parse(schema);
    schema_ = schema_.flatten();
    grand_schema_ = selected_schema;
    is_schemas_fetched_ = false;
}

void
ecb::YjSchema::normalize(json& yaml_data)
{
    for (const auto& schema_entry : schema_.items())
    {
        // quick check if this is a normalize key
        if (schema_entry.key().find("normalize") == std::string::npos)
            continue;

        std::smatch key_to_normalize;
        if (std::regex_search(schema_entry.key(), key_to_normalize,
            std::regex(R"(.*\/(.*)\/(?=(normalize)))")))
        {
            const auto key_ptr = ecb::yj_common::generate_json_pointer(key_to_normalize[1].str());

            for (const auto& val : schema_entry.value())
            {
                const auto split = ecb::yj_common::tokenize(
                        val,
                        ecb::yj_common::REGEX_token_sep_space);

                // skip if there is nothing to do
                if (yaml_data.contains(key_ptr) == false || split.size() < 2)
                    continue;

                // normalize (string=* types
                if ( (split[0].find("\(string") == 0) && yaml_data[key_ptr].is_string())
                {
                    for (auto norm_values = std::next(split.cbegin()) ; norm_values != split.cend();
                        ++norm_values)
                    {
                        std::smatch norm_pair;
                        std::regex_search(*norm_values, norm_pair, REGEX_norm_pair);

                        std::string from_yaml = yaml_data[key_ptr];
                        std::string from_scheme = norm_pair[1];

                        // remove whitespaces
                        if (split[0] == "(string_remove_whitespaces=integer)")
                            ecb::yj_common::replace_substring(from_yaml, " ", "");

                        // use lower casing for comparison
                        ecb::yj_common::to_lower(from_yaml);
                        ecb::yj_common::to_lower(from_scheme);

                        if (from_yaml == from_scheme)
                        {
                            if ((split[0] == "(string=integer)") || (split[0] == "(string_remove_whitespaces=integer)"))
                                yaml_data[key_ptr] = std::stoi(norm_pair[2]);

                            if (split[0] == "(string=string)")
                                yaml_data[key_ptr] = norm_pair[2];

                            if ((split[0] == "(string=boolean)") && ((norm_pair[2] == "true") || (norm_pair[2] == "True")))
                                yaml_data[key_ptr] = true;

                            if ((split[0] == "(string=boolean)") && ((norm_pair[2] == "false") || (norm_pair[2] == "False")))
                                yaml_data[key_ptr] = false;

                            break;
                        }
                    }
                }

                // normalize (integer=boolean)
                if ((split[0] == "(integer=boolean)") && yaml_data[key_ptr].is_number_integer())
                {
                    for (auto norm_values = std::next(split.cbegin()) ; norm_values != split.cend();
                        ++norm_values)
                    {
                        std::smatch norm_pair;
                        std::regex_search(*norm_values, norm_pair, REGEX_norm_pair);

                        int from_yaml = yaml_data[key_ptr];
                        std::string from_scheme_str = norm_pair[1];
                        int from_scheme = std::stoi(norm_pair[1]);

                        if (from_yaml == from_scheme)
                        {
                            if ((split[0] == "(integer=boolean)") && ((norm_pair[2] == "true") || (norm_pair[2] == "True")))
                                yaml_data[key_ptr] = true;

                            if ((split[0] == "(integer=boolean)") && ((norm_pair[2] == "false") || (norm_pair[2] == "false")))
                                yaml_data[key_ptr] = false;

                            break;
                        }
                    }
                }
            }
        }
    }
}

void
ecb::YjSchema::check_min_max_ranges(nlohmann::json& json)
{
    for (const auto& schema_entry : schema_.items())
    {
        // check "min" range
        if ((schema_entry.key().find("/min") == std::string::npos)
            && schema_entry.key().find("/max") == std::string::npos)
            continue;
        else
        {
            const bool isMax = (schema_entry.key().find("/max") != std::string::npos);
            const std::string::size_type key_end = schema_entry.key().rfind("/");
            const std::string::size_type key_start = schema_entry.key().rfind("/", key_end - 1);
            const std::string key = schema_entry.key().substr(key_start + 1, key_end - key_start - 1);
            const auto key_ptr = ecb::yj_common::generate_json_pointer(key);

            if ((json.contains(key_ptr)) == false || (json[key_ptr].is_number() == false))
                continue;

            if (isMax == true)
            {
                // max
                if (json[key_ptr] > schema_entry.value())
                {
                    snprintf(throw_msg, sizeof(throw_msg), "key: %s is greater than maximum value defined in schema",
                        key.c_str());
                    throw std::runtime_error(throw_msg);
                }
            }
            else
            {
                // min
                if (json[key_ptr] < schema_entry.value())
                {
                    snprintf(throw_msg, sizeof(throw_msg), "key: %s is less than minimum value defined in schema",
                        key.c_str());
                    throw std::runtime_error(throw_msg);
                }
            }
        }
    }
}

void
ecb::YjSchema::remove_undefined_keys(nlohmann::json& cfg_data)
{
    auto flatten_cfg_data = cfg_data.flatten();
    nlohmann::json clean_cfg;

    // check each key of cfg_data if it is covered by the schema
    for (const auto& cfg_entry : flatten_cfg_data.items())
    {
        bool is_defined = false;

        for (const auto& used_schema : used_schemas_)
        {
            // get prefix identifier from schema
            std::string id = "/";
            id += used_schema;
            id += "/identifier";
            const std::string prefix = ecb::yj_common::cfg_key_to_json_key_string(schema_[id]);

            if (cfg_entry.key().find(prefix) != std::string::npos)
            {
                is_defined = true;
                break;
            }
        }

        if (is_defined == true)
            clean_cfg[cfg_entry.key()] = cfg_entry.value();
        else
            ecb::yj_common::log("warning: key is not specified in schema; value will be ignored: " +
                cfg_entry.key());
    }

    clean_cfg = clean_cfg.unflatten();
    cfg_data = std::move(clean_cfg);
}


void
ecb::YjSchema::add_default_value_from_key(
    nlohmann::json& cfg_data,
    const std::string& key)
{
    std::string find_key = "/schema/" + key + "/default";

    for (const auto& schema_entry : schema_.items())
    {
        if (std::regex_search(schema_entry.key(), std::regex(find_key)))
        {
            const auto cfg_key = ecb::yj_common::generate_json_pointer(key);

            if (cfg_data.contains(cfg_key) == false)
                cfg_data[cfg_key] = schema_entry.value();
        }
    }
}

void
ecb::YjSchema::add_schema_default_values(
    nlohmann::json& cfg_data)
{
    fetch_list_of_schemas(grand_schema_, cfg_data);

    for (const auto& schema : all_schemas_)
    {
        if ((schema.second == true) || is_subschema_defined(schema.first, cfg_data))
        {
            std::string identifier_key = schema.first + ".identifier";
            auto key = ecb::yj_common::cfg_key_to_json_key_string(identifier_key);

            std::string key_prefix = "";

            if (schema_.contains(key))
                key_prefix = schema_[key];

            for (const auto& schema_entry : schema_.items())
            {
                if (schema_entry.key().find(key_prefix) != std::string::npos)
                {
                    std::smatch match;

                    std::string find_key = schema.first + "/schema/(" + key_prefix + ".*)" + "/default";

                    if (std::regex_search(schema_entry.key(), match, std::regex(find_key)))
                    {
                        const auto cfg_key = ecb::yj_common::generate_json_pointer(match[1].str());

                        if (cfg_data.contains(cfg_key) == false)
                            cfg_data[cfg_key] = schema_entry.value();
                    }
                }
            }
        }
    }
}

void
ecb::YjSchema::check_datatypes(
    nlohmann::json& cfg_data)
{
    for (const auto& schema_entry : schema_.items())
    {
        std::smatch match;

        if (std::regex_search(schema_entry.key(), match,
            std::regex(R"(/schema/(.*)/type)")))
        {
            const auto key = ecb::yj_common::generate_json_pointer(match[1].str());

            if (cfg_data.contains(key))
            {
                bool is_valid = false;
                const std::string value = schema_entry.value();
                const auto valid_datatypes = ecb::yj_common::tokenize(
                        value,
                        ecb::yj_common::REGEX_token_sep_space);

                for (const auto& datatype : valid_datatypes)
                {
                    if (datatype == "string" && cfg_data[key].is_string())
                        is_valid = true;

                    if (datatype == "integer" && (cfg_data[key].is_number_integer()
                        || cfg_data[key].is_number_unsigned()))
                        is_valid = true;

                    if (datatype == "boolean" && cfg_data[key].is_boolean())
                        is_valid = true;

                    if (datatype == "boolean" && cfg_data[key].is_string())
                    {
                        std::string value = cfg_data[key].template get<std::string>();

                        if (value == "True")
                        {
                            cfg_data[key] = true;
                            is_valid = true;
                        }

                        if (value == "False")
                        {
                            cfg_data[key] = false;
                            is_valid = true;
                        }
                    }

                    if (datatype == "float" && cfg_data[key].is_number_float())
                        is_valid = true;

                    if (datatype == "float" && cfg_data[key].is_number_integer())
                    {
                        double temp = cfg_data[key].template get<double>();
                        cfg_data[key] = temp;
                        is_valid = true;
                    }

                    if (datatype == "list" && cfg_data[key].is_array())
                        is_valid = true;

                    if (is_valid == true)
                        break;
                }

                if ((valid_datatypes.size() > 0) && (is_valid == false))
                    throw std::runtime_error("key: " + key.to_string() + " is not of datatype: " +
                        value);
            }
        }
    }
}

void
ecb::YjSchema::check_schema(const std::string& selected_schema, nlohmann::json& cfg_data)
{
    fetch_list_of_schemas(selected_schema, cfg_data);

    for (const auto& schema : all_schemas_)
    {
        if (schema.first.empty())
            continue;

        // is schema defined in schema file?
        auto id_key = ecb::yj_common::cfg_key_to_json_key_string(schema.first + ".identifier");

        if (schema_.contains(id_key) == false)
            throw std::runtime_error("unknown schema in schema file: " + schema.first);

        // check if there are keys in cfg_data that start with schema.identifier
        bool key_is_incomplete = is_incomplete_key(schema_[id_key], cfg_data);

        bool is_required = schema.second;

        if ((is_required == true) && (key_is_incomplete == false))
            throw std::runtime_error("configuration is missing required schema: " + schema.first);

        if ((key_is_incomplete == true) || (is_required == true))
        {
            if (is_required == true)
            {
                check_subschema(schema.first, cfg_data);
                used_schemas_.push_back(schema.first);
            }
            else
            {
                if (is_subschema_defined(schema.first, cfg_data) == true)
                {
                    check_subschema(schema.first, cfg_data);
                    used_schemas_.push_back(schema.first);
                }
            }
        }
    }
}

void
ecb::YjSchema::check_subschema(const std::string& subschema, nlohmann::json& cfg_data)
{
    for (const auto& schema : schema_.items())
    {
        if (schema.key().find(subschema) == std::string::npos)
            continue;

        // check if keys with required=true exist
        std::smatch match;

        if ((schema.key().find("required") != std::string::npos))
        {
            const auto REGEX_find_required = std::regex("/" + subschema + R"(/schema/(.*)/required)");

            if (std::regex_search(schema.key(), match, REGEX_find_required))
            {
                if ((schema.value() == true))
                {
                    const auto key = ecb::yj_common::generate_json_pointer(match[1].str());

                    if (cfg_data.contains(key) == false)
                    {
                        throw std::runtime_error("cannot find key: " + match[1].str() +
                            " required by schema: "
                            + subschema);
                    }
                }
            }
        }

        // check dependencies
        if (schema.key().find("dependencies") != std::string::npos)
        {
            const auto REGEX_find_dependencies = std::regex("/" + subschema + R"(/schema/(.*)/dependencies)");

            if (std::regex_search(schema.key(), match, REGEX_find_dependencies))
            {
                const auto key = ecb::yj_common::generate_json_pointer(match[1].str());

                if (cfg_data.contains(key))
                {
                    const auto dependencies = ecb::yj_common::tokenize(
                            schema.value().template get<std::string>(),
                            ecb::yj_common::REGEX_token_sep_space);

                    for (const auto& dependency : dependencies)
                    {
                        auto dep_key = ecb::yj_common::generate_json_pointer(dependency);

                        if (cfg_data.contains(dep_key) == false)
                        {
                            std::regex_search(schema.key(), match, std::regex(R"(/schema/(.*)/)"));
                            throw std::runtime_error("missing key dependency: \"" + match[1].str() +
                                "\" depends on \"" + dependency + "\"");
                        }
                    }
                }
            }
        }
    }
}

void
ecb::YjSchema::check_for_valid_keys(
    nlohmann::json& cfg_data)
{
    const auto flatten_cfg_data = cfg_data.flatten();
    std::string actual_cfg_key;
    std::string actual_schema_key;
    std::vector<std::string> valid_subkeys;

    // create a list of identifiers that shall be ignored due to
    // allowAnySubkey = true
    for (const auto& schema_entry : schema_.items())
    {
        if (schema_entry.key().find("/allowAnySubkey") != std::string::npos)
        {
            if (schema_entry.value() == true) // allowAnySubkey == true ?
            {
                // change /allowAnySubkey to /identifier
                std::string id_key = schema_entry.key();
                id_key.erase(id_key.size() - 14, id_key.size());
                id_key += "identifier";

                valid_subkeys.push_back(schema_[id_key]);
            }
        }
    }

    // check key with schema
    for (const auto& cfg_entry : flatten_cfg_data.items())
    {
        bool is_valid_key = false;

        // skip keys that are ok because of allowAnySubkey = true
        for (const auto& valid_subkey : valid_subkeys)
        {
            if (cfg_entry.key().find(valid_subkey) != std::string::npos)
            {
                is_valid_key = true;
                break;
            }
        }

        if (is_valid_key == true)
            continue;

        // check if key is defined somewhere in schema
        for (const auto& schema_entry : schema_.items())
        {
            actual_schema_key = ecb::yj_common::cfg_key_to_json_key_string(schema_entry.key());
            actual_cfg_key = "/schema" + cfg_entry.key();

            // keys point to array elements end with /0, /1 ... remove this
            auto pos_last_slash = actual_cfg_key.rfind("/");
            std::string::iterator it = actual_cfg_key.begin();
            std::advance(it, pos_last_slash + 1);

            if ((pos_last_slash != std::string::npos) && std::any_of(it, actual_cfg_key.end(), ::isdigit))
                actual_cfg_key.erase(it, actual_cfg_key.end());


            if (actual_schema_key.find(actual_cfg_key) != std::string::npos)
            {
                is_valid_key = true;
                break;
            }
        }

        if (is_valid_key == false)
            throw std::runtime_error("unknown key: " + cfg_entry.key());
    }
}

std::string
ecb::YjSchema::find_grand_schema_prefix(const std::string& selected_schema,
    nlohmann::json& cfg_data)
{
    std::string ret_val;
    std::smatch condition_key_value;
    std::smatch prefix_and_condition;

    for (const auto& schema : schema_.items())
    {
        const std::string rege = R"(^(\/grandSchema\/)" + selected_schema + R"(\/(.*)\/)(.*))";
        const auto REGEX_find_prefix_and_condition = std::regex(rege);

        if (std::regex_search(schema.key(), prefix_and_condition,
            REGEX_find_prefix_and_condition))
        {
            // prefix_and_condition[1] = /grandSchema/axis/axis.abc=0/ -> prefix
            // prefix_and_condition[2] = axis.type=0 -> condition
            const std::string condition = prefix_and_condition[2];


            if (std::regex_search(condition, condition_key_value, std::regex(R"((.+)=(.+))")))
            {
                // condition_key_value[1] = axis.type -> key
                // condition_key_value[2] = 0 -> value
                const auto key = ecb::yj_common::generate_json_pointer(condition_key_value[1]);

                if (cfg_data.contains(key) == true)
                {
                    if (cfg_data[key].is_number_integer()
                        && ((std::to_string(cfg_data[key].template get<int>()) == condition_key_value[2])))
                    {
                        ret_val = prefix_and_condition[1];
                        break;
                    }

                    if (cfg_data[key] == condition_key_value[2])
                    {
                        ret_val = prefix_and_condition[1];
                        break;
                    }
                }
            }
        }
    }

    return ret_val;
}

bool
ecb::YjSchema::is_incomplete_key(
    std::string identifier,
    nlohmann::json& cfg_data)
{
    auto flatten_cfg_data = cfg_data.flatten();
    std::string id = ecb::yj_common::cfg_key_to_json_key_string(identifier);
    id += "/";

    bool is_found = false;

    for (const auto& cfg_entry : flatten_cfg_data.items())
    {
        if (cfg_entry.key().find(id) == 0)
        {
            is_found = true;
            break;
        }
    }

    return is_found;
}

bool
ecb::YjSchema::is_subschema_defined(const std::string& schema, nlohmann::json& cfg_data)
{
    bool is_defined = false;
    std::string value;

    // check if schema is defined in schema file
    auto id_key = ecb::yj_common::cfg_key_to_json_key_string(schema + ".identifier");

    if (schema_.contains(id_key) == false)
        throw std::runtime_error("unknown schema in schema file: " + schema);

    value = schema_[id_key];
    is_defined = is_incomplete_key(value, cfg_data);

    return is_defined;
}

void
ecb::YjSchema::fetch_list_of_schemas(const std::string& grand_schema, nlohmann::json& cfg_data)
{
    if (is_schemas_fetched_ == false)
    {
        is_schemas_fetched_ = true;

        std::string schema_key_prefix = find_grand_schema_prefix(grand_schema, cfg_data);
        const std::string required_key = schema_key_prefix + "required";
        const std::string optional_key = schema_key_prefix + "optional";

        std::vector<std::string> required_schemas;

        if (schema_.contains(required_key))
            required_schemas = ecb::yj_common::tokenize(
                    schema_[required_key].template get<std::string>(),
                    ecb::yj_common::REGEX_token_sep_space);

        std::vector<std::string> optional_schemas;

        if (schema_.contains(optional_key))
            optional_schemas = ecb::yj_common::tokenize(
                    schema_[optional_key].template get<std::string>(),
                    ecb::yj_common::REGEX_token_sep_space);

        all_schemas_.reserve(required_schemas.size() + optional_schemas.size());

        for (const auto& req : required_schemas)
            all_schemas_.emplace_back(req, true);

        for (const auto& opt : optional_schemas)
            all_schemas_.emplace_back(opt, false);
    }
}

