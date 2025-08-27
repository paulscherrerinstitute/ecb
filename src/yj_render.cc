//
// ECB - render templates using data from yaml configuration
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

#include <inja.hpp>
#include <iterator>
#include <regex>

#include "yj_common.h"
#include "yj_render.h"

using namespace inja;
using nlohmann::json;


std::string
ecb::YjRender::render(
    const std::string& filename, const std::string& template_dir, json& data)
{
    std::ifstream template_content(filename);

    if (!template_content)
        throw std::runtime_error("template file not found: " + filename);

    return render(template_content, template_dir, data);
}

std::string
ecb::YjRender::render(
    std::istream& template_content, const std::string& template_dir,
    nlohmann::json& data)
{
    std::string line;
    std::string preprocessed_template;
    std::map<std::string, nlohmann::json> flatten_data = data.flatten();

    while (std::getline(template_content, line))
        preprocess_line(line, preprocessed_template, template_dir, flatten_data, 1);

    preprocessed_template.pop_back(); // remove last newline
    std::string rendered_template = "";

    try
    {
        inja::Environment env;
        env.set_trim_blocks(true);
        rendered_template = env.render(preprocessed_template, data);
    }
    catch (const json::exception& e)
    {
        std::cout << e.what() << '\n';
    }
    catch (const inja::InjaError&  e)
    {
        size_t line_number = 1;
        size_t start_index = 0;
        size_t stop_index = 0;
        bool found_start = false;

        std::cout << "== ECB: YAML ===================" << std::endl;
        std::cout << data.dump(2) << std::endl;

        std::cout << "== ECB: INJA ===================" << std::endl;
        std::cout << e.what() << std::endl;

        for (size_t i = 0 ; i < preprocessed_template.length() ; ++i)
        {
            if (preprocessed_template[i] == '\n')
                line_number++; ;

            if ((line_number == (e.location.line)) && (found_start == false))
            {
                start_index = i;
                found_start = true;
            }

            if (found_start)
            {
                if (line_number < (e.location.line + 8))
                    stop_index = i;
                else
                    break;
            }
        }

        std::cout << std::endl << preprocessed_template.substr(start_index,
                (stop_index - start_index)) << std::endl;
        throw e;
    }

    return rendered_template;
}


void
ecb::YjRender::transform_is_defined(
    std::string& line, const std::map<std::string, nlohmann::json>& data)
{

start:
    auto split = ecb::yj_common::tokenize(line, ecb::yj_common::REGEX_token_sep_space);

    const auto REGEX_convert_to_key = std::regex(R"(^\(|^|\.)");
    bool is_defined = false;
    bool is_existing_key = false;

    auto iter  = std::find(split.begin(), split.end(), "is");
    auto iter_p1 = std::next(iter, 1); // iter plus one
    auto iter_p2 = std::next(iter, 2); // iter plus two

    if ((iter != split.cend()) && (iter_p2 != split.cend()))
    {
        if ((iter_p1->compare("defined") == 0) || iter_p1->compare("defined)") == 0)
        {
            is_defined = true;
            goto found;
        }

        if ((iter_p1->compare("not") == 0) && ((iter_p2->compare("defined") == 0)
                || (iter_p2->compare("defined)") == 0)))
            goto found;
    }

    return;

found:
    auto iter_m1 = std::prev(iter); // iter minus one
    const std::string found_key = std::regex_replace(*iter_m1, REGEX_convert_to_key, "/");

    // check if key exists, match also incomplete keys
    for (const auto& entry : data)
    {
        if ((entry.first.rfind(found_key, 0) == 0) || (entry.first == (found_key + "/0")))
        {
            is_existing_key = true;
            break;
        }
    }

    if (is_defined == is_existing_key)
        *iter_m1 = "true";
    else
        *iter_m1 = "false";

    auto iter_end = iter_p2;

    if (!is_defined)
        ++iter_end;

    split.erase(iter, iter_end);

    line = "";

    for (auto it = split.cbegin() ; it != split.cend(); ++it)
    {
        line += *it;

        if ((it + 1) != split.cend())
            line += " ";
    }

    goto start;
}

void
ecb::YjRender::transform_is_string(std::string& line)
{
    const auto REGEX_find_is_string = std::regex(R"(([\w.]+)\s+(?=(is\s+string)))");
    const auto REGEX_replace_is_string = std::regex(R"(([\w.]+\s+is\s+string))");

    for (std::smatch match ; std::regex_search(line, match, REGEX_find_is_string);)
        line = std::regex_replace(line, REGEX_replace_is_string,
                ("isString(" + match[1].str() + ")"), std::regex_constants::format_first_only);
}

void
ecb::YjRender::transform_loop_index0(std::string& line)
{
    const auto REGEX_loop_index0 = std::regex(R"(loop.index0)");

    for (std::smatch match ; std::regex_search(line, match, REGEX_loop_index0);)
        line = std::regex_replace(line, REGEX_loop_index0, "loop.index",
                std::regex_constants::format_first_only);
}

void
ecb::YjRender::transform_default(std::string& line)
{
    const auto REGEX_find_default_pipe = std::regex(R"(([\w.]+)(?=\|default\((.+?)\)))");
    const auto REGEX_replace_default_pipe = std::regex(R"([\w.]+\|default\(.+?\))");

    for (std::smatch match ; std::regex_search(line, match, REGEX_find_default_pipe);)
    {
        const std::string replace_text = "default(" + match[1].str() + ", " +
            match[2].str() + ")";
        line = std::regex_replace(line, REGEX_replace_default_pipe, replace_text,
                std::regex_constants::format_first_only);
    }
}

void
ecb::YjRender::transform_int_cast(
    std::string& line, const std::map<std::string, nlohmann::json>& data)
{
    const auto REGEX_find_int_pipe = std::regex(R"(([\w.]+)(?=\|int))");
    const auto REGEX_replace_int_pipe = std::regex(R"(([\w.]+\|int))");

    for (std::smatch match ; std::regex_search(line, match, REGEX_find_int_pipe);)
    {
        const std::string found_key = ecb::yj_common::cfg_key_to_json_key_string(match[1].str());
        std::string replace_text = match[1];

        if (auto it = data.find(found_key); (it != data.end()) && (it->second.is_boolean()))
            (it->second.get<bool>() == true) ? (replace_text = "1") : (replace_text = "0");

        line = std::regex_replace(line, REGEX_replace_int_pipe, replace_text,
                std::regex_constants::format_first_only);
    }
}

void
ecb::YjRender::transform_default_int_cast(
    std::string& line, const std::map<std::string, nlohmann::json>& data)
{
    const auto REGEX_find_default_int_pipe = std::regex(R"(((default\((.+?),(.+?)\))\|int))");
    const auto REGEX_replace_default_int_pipe = std::regex(R"(default\(.+?\)\|int)");

    for (std::smatch match ; std::regex_search(line, match, REGEX_find_default_int_pipe);)
    {
        std::string replace_text = match[2];

        // check if found key is a boolean
        const std::string found_key = ecb::yj_common::cfg_key_to_json_key_string(match[3].str());

        if (auto it = data.find(found_key); (it != data.end()) && (it->second.is_boolean()))
        {
            if (it->second.get<bool>() == true)
                replace_text = "1";
            else
                replace_text = "0";
        }

        line = std::regex_replace(line, REGEX_replace_default_int_pipe, replace_text,
                std::regex_constants::format_first_only);

    }
}

void
ecb::YjRender::remove_float_cast(std::string& line)
{
    const auto REGEX_find_float_cast = std::regex(R"(([\w.]+)(?=\|float))");
    const auto REGEX_remove_float_cast = std::regex(R"(([\w.]+\|float))");

    for (std::smatch match ; std::regex_search(line, match, REGEX_find_float_cast);)
        line = std::regex_replace(line, REGEX_remove_float_cast, match[1].str(),
                std::regex_constants::format_first_only);
}

void
ecb::YjRender::preprocess_line(std::string& line,
    std::string& expanded_template, const std::string& template_base_dir,
    const std::map<std::string, nlohmann::json>& flatten_data, int call_count)
{
    if (call_count > ECMC_YJ_RENDER_MAX_INCLUDE_DEPTH)
        throw std::runtime_error("template: limit of nested includes is exceed. Limit: ECMC_YJ_RENDER_MAX_INCLUDE_DEPTH");

    // handle line without inja syntax
    if ((line.find(R"({%)") == std::string::npos) && (line.find(R"({{)") == std::string::npos))
    {
        yj_common::remove_whitespaces(line);

        if (line.length() != 0)
            expanded_template += line + "\n";

        return;
    }


    if (line.find("include") != std::string::npos)
    {
        const auto REGEX_find_include = std::regex(R"(^\s*\{\%\s+include\s+["'](.+)["']\s+\%\})");
        std::smatch match;

        std::regex_search(line, match, REGEX_find_include);

        // include statement found, so include the content of this file
        std::ifstream include_file(template_base_dir + "/" + match[1].str());

        if (!include_file)
            throw std::runtime_error("include file not found: " + match[1].str());

        std::string included_line;

        while (std::getline(include_file, included_line))
            preprocess_line(included_line, expanded_template, template_base_dir,
                flatten_data, call_count + 1);
    }
    else
    {
        bool contains_default = false;

        if (line.find(R"(default)") != std::string::npos)
            contains_default = true;

        if (line.find(R"(is)") != std::string::npos)
        {
            if (line.find(R"(defined)") != std::string::npos)
                transform_is_defined(line, flatten_data);

            if (line.find(R"(string)") != std::string::npos)
                transform_is_string(line);
        }

        if (line.find("index0") != std::string::npos)
            transform_loop_index0(line);

        if (contains_default)
            transform_default(line);

        if (line.find("|int") != std::string::npos)
            transform_int_cast(line, flatten_data);

        if (contains_default)
            transform_default_int_cast(line, flatten_data);

        if (line.find("|float") != std::string::npos)
            remove_float_cast(line);

        yj_common::remove_whitespaces(line);

        if (line.length() != 0)
            expanded_template += line + "\n";
    }
}
