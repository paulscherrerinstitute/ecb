//
// ECB - handle command line arguments
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

#include <iostream>
#include <string_view>
#include <vector>

#include "ecb_arg_handler.h"

using namespace ecb;

// Defines the valid command line arguments, along with the corresponding
// valid values. If the second field is empty, any value can be used.
const std::unordered_map<std::string, std::vector<std::string>> valid_args =
{
    {"--help", {""}},
    {"--yaml", {""}},
    {"--template", {""}},
    {"--templatedir", {""}},
    {"--schema", {"axis", "encoder", "plc"}},
    {"--schemafile", {""}},
    {"--action", {"build", "readkey", "updatekey"}},
    {"--output", {""}},
    {"--key", {""}},
    {"--value", {""}},
    {"--version", {""}},
};

const std::unordered_map<mode, std::vector<std::string>> valid_combinations =
{
    {mode::YJ_BUILD_CFG_TO_STDOUT, {"--yaml", "--schemafile", "--schema",  "--action", "--template", "--templatedir"}},
    {mode::YJ_BUILD_CFG_TO_FILE, {"--yaml", "--schemafile", "--schema",  "--action", "--template", "--templatedir", "--output"}},
    {mode::YJ_READ_KEY_TO_STDOUT, {"--yaml", "--action", "--key"}},
    {mode::YJ_READ_KEY_TO_FILE, {"--yaml", "--action", "--key", "--output"}},
    {mode::YJ_UPDATE_KEY, {"--yaml", "--action", "--key", "--value", "--output"}},
    {mode::YJ_UPDATE_KEY_TO_STDOUT, {"--yaml", "--action", "--key", "--value"}},
    {mode::BUILD_INFO, {"--version"}},
    {mode::HELP, {"--help"}},
};


ecb::mode
ArgHandler::get_mode(void)
{
    auto ret_val = mode::INVALID;

    // lambda
    auto check_combination = [&](ecb::mode mode, bool checkAction, const std::string& action)
    {
        if (const auto& valid_set = valid_combinations.find(mode); valid_set != valid_combinations.end())
        {
            size_t count = 0;

            for (const auto& necessary_args : valid_set->second)
            {
                if (const auto& search = args_.find(necessary_args) ; search != args_.end())
                    count++;
            }

            if (count == valid_set->second.size())
            {
                if ((checkAction == false) || (args_["--action"] == action))
                    ret_val = mode;
            }
        }
    };

    check_combination(mode::YJ_READ_KEY_TO_STDOUT, true, "readkey");
    check_combination(mode::YJ_READ_KEY_TO_FILE, true, "readkey");
    check_combination(mode::YJ_BUILD_CFG_TO_STDOUT, true, "build");
    check_combination(mode::YJ_BUILD_CFG_TO_FILE, true, "build");
    check_combination(mode::YJ_UPDATE_KEY_TO_STDOUT, true, "updatekey");
    check_combination(mode::YJ_UPDATE_KEY, true, "updatekey");
    check_combination(mode::BUILD_INFO, false, "updatekey");
    check_combination(mode::HELP, false, "updatekey");

    return ret_val;
}

bool
ArgHandler::set_argument(std::string_view arg, std::string_view value)
{
    bool ret_val = false;

    for (const auto& valid_arg : valid_args)
    {
        if (arg == valid_arg.first)
        {
            if (valid_arg.second[0] != "")
            {
                for (const auto& valid_value : valid_arg.second)
                {
                    if (value == valid_value)
                    {
                        std::string key{ arg };
                        args_[key] = value;
                        ret_val = true;
                        break;
                    }
                }
            }
            else
            {
                std::string key{ arg };
                args_[key] = value;
                ret_val = true;
                break;
            }
        }
    }

    return ret_val;
}

std::string
ArgHandler::get_yj_yaml_filename(void)
{
    std::string ret_val = "";

    if (auto it = args_.find("--yaml") ; it != args_.end())
        ret_val = args_["--yaml"];

    return ret_val;
}

std::string
ArgHandler::get_output_filename(void)
{
    std::string ret_val = "";

    if (auto it = args_.find("--output") ; it != args_.end())
        ret_val = args_["--output"];

    return ret_val;

}

std::string
ArgHandler::get_yj_key_value(void)
{
    std::string ret_val = "";

    if (auto it = args_.find("--key") ; it != args_.end())
        ret_val = args_["--key"];

    return ret_val;
}

std::string
ArgHandler::get_yj_value(void)
{
    std::string ret_val = "";

    if (auto it = args_.find("--value") ; it != args_.end())
        ret_val = args_["--value"];

    return ret_val;
}

std::string
ArgHandler::get_yj_schema_filename(void)
{
    std::string ret_val = "";

    if (auto it = args_.find("--schemafile") ; it != args_.end())
        ret_val = args_["--schemafile"];

    return ret_val;
}

std::string
ArgHandler::get_yj_schema(void)
{
    std::string ret_val = "";

    if (auto it = args_.find("--schema") ; it != args_.end())
        ret_val = args_["--schema"];

    return ret_val;
}

std::string
ArgHandler::get_yj_template_filename(void)
{
    std::string ret_val = "";

    if (auto it = args_.find("--template") ; it != args_.end())
        ret_val = args_["--template"];

    return ret_val;
}

std::string
ArgHandler::get_yj_template_dir(void)
{
    std::string ret_val = "";

    if (auto it = args_.find("--templatedir") ; it != args_.end())
        ret_val = args_["--templatedir"];

    return ret_val;
}
