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


#ifndef _YJ_COMMON_H_
#define _YJ_COMMON_H_

#include <string>
#include <vector>
#include <regex>

#include <json.hpp>


namespace ecb
{
namespace yj_common
{

// Predefined regular expressions for use with the `tokenize`
// function
extern const std::regex REGEX_token_sep_space;
extern const std::regex REGEX_token_sep_dot;

// Separates words in a string using the given regular expression as the
// delimiter.
std::vector<std::string> tokenize(
    std::string value,
    const std::regex regex_expr);

// Changes casing of given string to only lower case
void to_lower(
    std::string& value);

// replace all occurances of f in s with t
void replace_substring(
    std::string& s,
    const std::string& f,
    const std::string& t);

// remove leading and trailing whitesapces from line
void remove_whitespaces(
    std::string& line);

// add entry to output log
void log(
    std::string txt);
//
// Returns a JSON pointer to be used with the nlohmann::json.  The expected
// input key format is "a.b.c".
nlohmann::json::json_pointer generate_json_pointer(
    const std::string& key);

// Returns a key given in format "a.b.c" in the new format "/a/b/c".
std::string cfg_key_to_json_key_string(
    std::string key);

// write `data` to `filename`. If the parent path to `filename` does not exist,
// it will be created. Throws an exception if `filename` cannot be created.
void write_file(
    std::string& filename,
    std::string& data);
}
}

#endif // _YJ_COMMON_H_
