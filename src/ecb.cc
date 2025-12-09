//
// ECB - ecmc configuration builder
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
#include <string>

#include "ecb.h"
#include "ecb_arg_handler.h"
#include "yj_cfg.h"
#include "yj_common.h"


void ecb_run(int argc, char* argv[])
{
    auto OBJ_argparser = ecb::ArgHandler();

    OBJ_argparser.set_argument("--action", "build");

    if (argc > 2)
    {
        for (int i = 1 ; i < argc ; i = i + 2)
        {
            if (i + 1 < argc)
                OBJ_argparser.set_argument(argv[i], argv[i + 1]);
        }
    }
    else if (argc == 2)
        OBJ_argparser.set_argument(argv[1], "");

    auto OBJ_yj_cfg = ecb::YjConfiguration();

    switch (OBJ_argparser.get_mode())
    {
        case ecb::mode::YJ_READ_KEY_TO_STDOUT:
        {
            std::string output = OBJ_yj_cfg.read_key(OBJ_argparser.get_yj_yaml_filename(),
                    OBJ_argparser.get_yj_key_value());
            std::cout << output << std::endl;
            break;
        }

        case ecb::mode::YJ_READ_KEY_TO_FILE:
        {
            std::string output = OBJ_yj_cfg.read_key(OBJ_argparser.get_yj_yaml_filename(),
                    OBJ_argparser.get_yj_key_value());

            std::string filename = OBJ_argparser.get_output_filename();
            ecb::yj_common::write_file(filename, output);

            break;
        }

        case ecb::mode::YJ_UPDATE_KEY:
        {
            std::string output = OBJ_yj_cfg.update_key(
                    OBJ_argparser.get_yj_yaml_filename(),
                    OBJ_argparser.get_yj_key_value(),
                    OBJ_argparser.get_yj_value());

            if (output != "")
            {
                std::string filename = OBJ_argparser.get_output_filename();
                ecb::yj_common::write_file(filename, output);
            }

            break;
        }

        case ecb::mode::YJ_UPDATE_KEY_TO_STDOUT:
        {
            std::string output = OBJ_yj_cfg.update_key(
                    OBJ_argparser.get_yj_yaml_filename(),
                    OBJ_argparser.get_yj_key_value(),
                    OBJ_argparser.get_yj_value());

            std::cout << output << std::endl;
            break;
        }

        case ecb::mode::YJ_BUILD_CFG_TO_STDOUT:
        {
            std::string output = OBJ_yj_cfg.build(
                    OBJ_argparser.get_yj_yaml_filename(),
                    OBJ_argparser.get_yj_schema_filename(),
                    OBJ_argparser.get_yj_schema(),
                    OBJ_argparser.get_yj_template_filename(),
                    OBJ_argparser.get_yj_template_dir());
            std::cout << output << std::endl;
            break;

        }

        case ecb::mode::YJ_BUILD_CFG_TO_FILE:
        {
            std::string output = OBJ_yj_cfg.build(
                    OBJ_argparser.get_yj_yaml_filename(),
                    OBJ_argparser.get_yj_schema_filename(),
                    OBJ_argparser.get_yj_schema(),
                    OBJ_argparser.get_yj_template_filename(),
                    OBJ_argparser.get_yj_template_dir());

            if (output != "")
            {
                std::string filename = OBJ_argparser.get_output_filename();
                ecb::yj_common::write_file(filename, output);
            }

            break;
        }

        case ecb::mode::BUILD_INFO:
        {
            std::cout << "ECB - ecmc configuration builder" << std::endl
                << "--------------------------------" << std::endl
                << "version: " << MAKEFILE_BUILD_VERSION << std::endl
                << "build  : #" << MAKEFILE_BUILD_NUMBER << MAKEFILE_BUILD_DIRTY << std::endl
                << "date   : " << MAKEFILE_BUILD_DATE << std::endl
                << "hash   : " << MAKEFILE_BUILD_HASH << std::endl << "---" << std::endl;
            break;
        }

        case ecb::mode::HELP:
        case ecb::mode::INVALID:
        {
            std::cout << ecb::help_text;
            break;
        }
    }
}


int main(int argc, char* argv[])
{
    ecb_run(argc, argv);
}
