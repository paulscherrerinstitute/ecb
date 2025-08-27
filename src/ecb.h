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

#ifndef _ECB_H_
#define _ECB_H_

#include <string>

namespace ecb
{
const std::string help_text = "\nECB - ecmc configuration builder\n"
    "\n"
    "Usage:\n"
    "  ecb [--action build] --yaml YFILE --schema SCHEMA --schemafile SFILE\n"
    "      --template TFILE --templatedir TDIR [--output OFILE]\n"
    "\n"
    "  ecb --action readkey --yaml YFILE --key KEY [--output OFILE]\n"
    "  ecb --action updatekey --yaml YFILE --key KEY --value VAL [--output OFILE]\n"
    "\n"
    "Options:\n"
    "  --action (build|readkey|updatekey)\n"
    "      Action to run, valid options are 'build' (default), 'readkey' or\n"
    "      'updatekey'. To build configurations use 'build'. The 'readkey' option\n"
    "      reads the specified KEY in YFILE. The 'updatekey' option updates the\n"
    "      value of KEY with VAL if KEY exists in YFILE.\n"
    "  --help\n"
    "      Show this text.\n"
    "  --key KEY\n"
    "      Read or update the value of KEY. If the key doesn't exist in YFILE,\n"
    "      then ECB just quits.\n"
    "  --output OFILE\n"
    "      Write the rendered Jinja2 template to OFILE. If this option is not\n"
    "      specified, the rendered template will be written to stdout.\n"
    "  --schema SCHEMA\n"
    "      Specifie schema to use, valid options are axis, encoder or plc.\n"
    "  --schemafile SFILE\n"
    "      Filename of ECB schema file.\n"
    "  --template TFILE\n"
    "      Filename of Jinja2 template.\n"
    "  --templatedir TDIR\n"
    "      TDIR specifies the directory where the Jinja2 templates are located.\n"
    "      If a Jinja2 template includes another template, then it is expected to be\n"
    "      found in this directory\n"
    "  --value VAL\n"
    "      Update the value of KEY specified with the --key option to VAL.\n"
    "  --version\n"
    "      Show version.\n"
    "  --yaml YFILE\n"
    "      Filename of YAML configuration.\n"
    "\n";
}

#endif // _ECB_H_
