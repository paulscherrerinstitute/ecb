#ifndef _ECB_H_
#define _ECB_H_

#include <string>

namespace ecb
{

const std::string help_text = "usage: ./ecb\n"
    "\n"
    "--action\t build (default), readkey, updatekey\n"
    "--backend\t yaml (default)\n"
    "--help\t\t this text\n"
    "--schema\t filename of schema file\n"
    "--template\t filename of template\n"
    "--templatedir\t path to templates\n"
    "--version\t show version\n"
    "--yaml\t\t filename of yaml configuration\n"
    "\nwhen --action updatekey or --action readkey\n\n"
    "--key\t\t key to read/write\n"
    "--value\t\t value to write\n"
    "--output\t output filename; if not provided, defaults to stdout\n";
}

#endif // _ECB_H_
