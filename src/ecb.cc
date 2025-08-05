#include <fstream>
#include <iostream>
#include <string>

#include "ecb.h"
#include "ecb_arg_handler.h"
#include "yj_cfg.h"
#include "yj_common.h"

int main(int argc, char* argv[])
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
                << "version: " << MAKEFILE_VERSION << std::endl
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

