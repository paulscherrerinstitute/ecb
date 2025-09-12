//
// ECB - high level yaml functions
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

#include <json.hpp>

#include "yj_cfg.h"
#include "yj_render.h"
#include "yj_schema.h"
#include "yj_yaml.h"

std::string
ecb::YjConfiguration::read_key(
    const std::string& filename_yaml,
    const std::string& key)
{
    auto OBJ_yaml = ecb::YjYaml();
    std::string value = "";
    value = OBJ_yaml.read_yaml_key(filename_yaml, key);
    return value;
}

std::string
ecb::YjConfiguration::update_key(
    const std::string& filename_yaml,
    const std::string& key,
    const std::string& value)
{
    auto OBJ_yaml = ecb::YjYaml();
    std::string ret_val = "";
    ret_val = OBJ_yaml.update_yaml_key(filename_yaml, key, value);
    return ret_val;
}

std::string
ecb::YjConfiguration::build(
    const std::string& filename_yaml,
    const std::string& filename_schema,
    const std::string& selected_schema,
    const std::string& filename_template,
    const std::string& template_dir)
{
    auto OBJ_yaml = ecb::YjYaml();
    auto OBJ_schema = ecb::YjSchema(filename_schema, selected_schema);
    auto OBJ_render = ecb::YjRender();

    nlohmann::json cfg_data = nlohmann::json();
    OBJ_yaml.read_yaml(filename_yaml, cfg_data);

    // pre-eval axis.type
    if (selected_schema == "axis")
        OBJ_schema.add_default_value_from_key(cfg_data, "axis.type");
    else if ((selected_schema == "plc") || (selected_schema == "encoder"))
        cfg_data["/meta/schemaNumber"_json_pointer] = 0;

    OBJ_schema.normalize(cfg_data);

    OBJ_schema.add_schema_default_values(cfg_data);
    OBJ_schema.check_datatypes(cfg_data);
    OBJ_schema.normalize(cfg_data);

    if (selected_schema == "axis")
        cfg_data["/meta/schemaNumber"_json_pointer] = cfg_data["/axis/type"_json_pointer];

    OBJ_schema.check_min_max_ranges(cfg_data);
    OBJ_schema.check_schema(selected_schema, cfg_data);
    OBJ_schema.check_for_valid_keys(cfg_data);
    OBJ_schema.remove_undefined_keys(cfg_data);

    const std::string configuration = OBJ_render.render(filename_template, template_dir, cfg_data);

    return configuration;
}

