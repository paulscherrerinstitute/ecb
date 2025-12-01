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

#ifndef _YJ_SCHEMA_H_
#define _YJ_SCHEMA_H_

#include <istream>
#include <nlohmann/json.hpp>
#include <string>

namespace ecb
{
class YjSchema
{
public:

    // Loads the schema specified by `schema` or `filename_schema` and
    // initializes the object for further use with the public methods.
    YjSchema(
        std::istream& schema,
        const std::string& selected_schema);

    YjSchema(
        std::string filename_schema,
        const std::string& selected_schema);


    // This function adds the default value for `key` to `cfg_data`, but only
    // if the following conditions are met:
    //
    //   1. A `default` value is specified for `key` in the schema file.
    //   2. The `key` is not already defined in `cfg_data`
    //
    // If these conditions are not fullfilled, the function returns without
    // modifying `cfg_data`.
    void add_default_value_from_key(
        nlohmann::json& cfg_data,
        const std::string& key);


    // This function adds the default values to `cfg_data` for schemas marked
    // as required or optional in the schema file.
    //
    // General conditions:
    //
    //   - A default value must be specified for a key in the schema file.
    //   - The `key` doesn't exist in `cfg_data`.
    //   - Required schemas: all default values are added as long as the key
    //     does not exist.
    //   - Optional schemas: all default values are added as long as the key
    //     does not exist and at least one key of this schema is defined.
    //
    // If the above conditions are not fullfilled, `cfg_data` is not modified.
    void add_schema_default_values(
        nlohmann::json& cfg_data);


    // This function verifies that the datatypes in `cfg_data` match those
    // defined in the schema file.  If there is a mismatch, the function throws
    // an exception to indicate an error. If all datatypes are ok, the function
    // just returns.
    void check_datatypes(
        nlohmann::json& cfg_data);


    // Checks if all keys in `cfg_data`are defined in the schema. If any key is
    // not defined, this function throws an exception. Keys which are covered
    // by `allowAnySubkey=true` are ignored.  If all keys are defined in the
    // schema, this function just returns without throwing an exception.
    void check_for_valid_keys(
        nlohmann::json& cfg_data);


    // This function checks all required and optional schemas defined
    // for `selected_schema`. The check ensures that all `required` keys
    // are defined and all `dependencies` are fulfilled. If any condition
    // is not met, the function throws an exception. If all conditions are
    // satisfied, the function completes successfully and returns.
    void check_schema(
        const std::string& selected_schema,
        nlohmann::json& cfg_data);


    // Replaces a key's value if `normalize` is defined in the schema file for
    // that key, and the key's value matches one of the `normalize` values.
    // String comparisons for normalization are case-insensitive.
    //
    // Examples of what `noramlize` can be used for:
    //   - (string=boolean): yes/no -> true/false ; Yes/No -> true/false ...
    //   - (string=integer): real -> 1 ; virtual -> 2 ...
    //   - (string=string): csp -> CSP ; cSp -> CSP ...
    //
    void normalize(nlohmann::json& json);

    // This function checks if the value of a key is within the
    // defined min or max values. If not, an exception is thrown. The check
    // is only performed if the value of the key is of type integer or float;
    // otherwise the check is skipped.
    void check_min_max_ranges(nlohmann::json& json);

    // Removes entries in 'cfg_data' that are not defined in the schema. For
    // example, if the configuration contains a drive section but the axis is
    // virtual, then the drive section is removed from the configurtation.
    //
    // Remarks:
    //   - This function does not update the existing `cfg_data`. Instead
    //     it creates a new json object and moves it to `cfg_data`.
    //   - `check_schema()` must be called before calling this function.
    //
    void remove_undefined_keys(nlohmann::json& cfg_data);


private:
    bool is_schemas_fetched_;
    nlohmann::json schema_;
    std::string grand_schema_;
    std::vector<std::pair<std::string, bool>> all_schemas_;
    std::vector<std::string> used_schemas_;
    char throw_msg[350];

    // Checks if the given key is an incomplete key in `cfg_data`. Returns true of the key is
    // incomplete, otherwise false. An incomplete key is a key that can have
    // more levels added. Only more levels are allowed at the end and not at
    // the beginning of the key.
    //
    // Example:
    //    X contains only one key: "a.b.c"
    //
    //    is_complete_key("a.b", X) -> true
    //    is_complete_key("b.c", X) -> false
    //    is_complete_key(a.b.c", X) -> false
    bool is_incomplete_key(
        std::string identifier,
        nlohmann::json& cfg_data);


    // Checks if the given schema is present in schema file and if the
    // configuration data include it.  Returns true if the subschema exists,
    // otherwise false.
    bool is_subschema_defined(
        const std::string& schema,
        nlohmann::json& cfg_data);


    // Returns the prefix of the key for the selected grand schema if:
    //
    //     1. the prefix exists in the schema file
    //     2. the key's condition is true
    //
    // An empty string is returned if either of the conditions is not true,
    // otherwise, returns the prefix in the format:
    // `/grandSchema/selected_schema/condition`. The condition is defined
    // as `key=value` where both values are in `cfg_data`
    //
    // Example:
    //     `selected_schema` shall be `axis` and the condition is `axis.type=1`.
    //     Then `/grandSchema/axis/axis.type=1/ is returned, but only if `cfg_data`
    //     contains a key `axis.type` with the value 1.
    std::string find_grand_schema_prefix(
        const std::string& selected_schema,
        nlohmann::json& cfg_data);


    // Checks if keys with `required=true` and dependencies of `subschema` are
    // defined. Throws an exception of any check fails.  If all checks pass,
    // this function returns without throwing an exception.
    void check_subschema(
        const std::string& subschema,
        nlohmann::json& cfg_data);


    // Fills `all_schemas_` with the schemas defined for `grand_schema`.
    // Fetches values only once; subsequent calls return without doing
    // anything.
    void fetch_list_of_schemas(
        const std::string& grand_schema,
        nlohmann::json& cfg_data);
};
}

#endif // _YJ_SCHEMA_H_
