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

#ifndef _YJ_RENDER_H_
#define _YJ_RENDER_H_

#include <istream>
#include <json.hpp>
#include <string>

#define ECMC_YJ_RENDER_MAX_INCLUDE_DEPTH 5

namespace ecb
{
class YjRender
{
public:

    // Renders the Jinja2 template provided in `templateContent` / `filename`.
    // First the template is preprocessed (see `preprocess_line`), and then
    // Inja is called. If Inja throws an exception, the corresponding context
    // is printed to stdout. The returned string is the final configuration
    // that is ready to be used by EPICS/ECMC.
    std::string render(
        std::istream& templateContent,
        const std::string& templateDir,
        nlohmann::json& data);

    std::string render(
        const std::string& filename,
        const std::string& templateDir,
        nlohmann::json& data);

private:

    // Preprocesses the given line and adds the result to `expanded_template`.
    // This function handles `include` statements in the Jinja2 templates and
    // applies all transform functions to each line.  Note: this function uses
    // recursion calls to include files. The maximum recursion/include depth is
    // set in `ECMC_YJ_RENDER_MAX_IN´CLUDE_DEPTH`.
    void preprocess_line(
        std::string& line,
        std::string& expanded_template,
        const std::string& template_dir,
        const std::map<std::string, nlohmann::json>& flatten_data, int call_count);

    // Replaces all occurrences of `K|float` with `K` in the given line. Casts
    // to float are not needed and because Inja do not support pipe operations
    // it is just removed. If no occurrences are found, the line remains
    // unchanged.
    void remove_float_cast(
        std::string& line);


    // Remove leading and trailing whitespaces from line.
    void remove_whitespaces(
        std::string& line);

    // Replaces all occurrences of `K|default(X)`with `default(K, X)` in the
    // given line. This transformation is necessary because Inja does not
    // support pipe operations. If no occurrences are found, the line remains
    // unchanged.
    void transform_default(
        std::string& line);


    // Replaces all occurrences of `K|int` in the given line according to the
    // following logic:
    //
    // `K|int` where VAL(K)=true -> 1
    // `K|int` where VAL(K)=false -> 0
    // `K|int` -> K
    //
    // If K is a boolean, it is converted to 1 or 0 based on its value. In all
    // other cases, the pipe is replaced with the key (K) before the pipe.
    // Floats are not rounded or truncated.  This transformation is necessary
    // because Inja does not support pipe operations. If no occurrences are
    // found, the line remains unchanged.
    void transform_int_cast(
        std::string& line,
        const std::map<std::string, nlohmann::json>& data);


    //
    //
    //
    void transform_default_int_cast(
        std::string& line,
        const std::map<std::string, nlohmann::json>& data);


    // Replaces all occurrences of `is defined` or `is not defined` in the
    // given line. The expression is replaced with `true/false`, depending on
    // whether the given key is defined or not. If no occurrences are found,
    // the line remains unchanged.
    void transform_is_defined(
        std::string& line,
        const std::map<std::string, nlohmann::json>& data);


    // Replaces all occurrences of `is string X` with `isString(X)` in the
    // given line. The expression `is string` is not supported by Inja. If no
    // occurrences are found, the line remains unchanged.
    void transform_is_string(
        std::string& line);


    // Replaces all occurrences of `loop.index0` with `loop.index` in the given
    // line. This is necessary due to different conventions between Jinja2 and
    // Inja. If no occurrences are found, the line remains unchanged.
    void transform_loop_index0(
        std::string& line);
};
}

#endif // _YJ_RENDER_H_
