//
// ECB - functions common to all modules
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
#include <filesystem>

#include "yj_common.h"

const std::regex ecb::yj_common::REGEX_token_sep_dot = std::regex(R"(\.+)");
const std::regex ecb::yj_common::REGEX_token_sep_space = std::regex(
        R"(\s+)");


std::vector<std::string>
ecb::yj_common::tokenize(std::string value, const std::regex regex_expr)
{
    auto ret_val = std::vector<std::string> (
            std::sregex_token_iterator
    {
        value.cbegin(), value.cend(),
        regex_expr, -1
    },
    std::sregex_token_iterator{});

    return ret_val;
}

void
ecb::yj_common::to_lower(std::string& value)
{
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char c)
    {
        return std::tolower(c);
    });
}

void
ecb::yj_common::replace_substring(std::string& s, const std::string& f, const std::string& t)
{
    if (f.empty())
        return;

    for (auto pos = s.find(f); pos != std::string::npos;
        s.replace(pos, f.size(), t), pos = s.find(f, pos + t.size()))
    {
    }
}

void
ecb::yj_common::remove_whitespaces(std::string& line)
{
    // leading
    size_t start = line.find_first_not_of(" \n\r\t\f\v");
    line = (start == std::string::npos) ? "" : line.substr(start);

    // trailing
    size_t end = line.find_last_not_of(" \n\r\t\f\v");
    line = (end == std::string::npos) ? "" : line.substr(0, end + 1);
}

void
ecb::yj_common::log(const std::string txt)
{
    std::cout << "<-ECB-> " << txt << std::endl;
}

std::string
ecb::yj_common::cfg_key_to_json_key_string(
    std::string key)
{
    std::string ret_val;
    ret_val.reserve(key.size() + 1);

    if (key.front() != '/')
        ret_val = "/";

    ret_val += key;
    ecb::yj_common::replace_substring(ret_val, ".", "/");

    return ret_val;
}

nlohmann::json::json_pointer
ecb::yj_common::generate_json_pointer(
    const std::string& key)
{
    nlohmann::json::json_pointer json_ptr(cfg_key_to_json_key_string(key));
    return json_ptr;
}

void
ecb::yj_common::write_file(std::string& filename, std::string& data)
{
    std::filesystem::path file(filename);

    if (file.has_parent_path())
    {
        std::filesystem::path dir(filename);
        std::filesystem::create_directories(dir.remove_filename());
    }

    std::ofstream out_file(file);

    if (out_file.is_open())
    {
        out_file << data;
        out_file.close();
    }
    else
        throw std::runtime_error("could not create file: " + filename);
}
