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

#include <epicsExport.h>
#include <iocsh.h>
#include <string.h>

#include "ecb.h"

#define MAX_ECB_ARGUMENT_NO 15
#define MAX_ECB_ARGUMENT_LENGTH 350

static const iocshArg ecbArgs = {"ecb args", iocshArgArgv};
static const iocshArg *ecbArgsArray[] = { &ecbArgs };
static const iocshFuncDef ecbDef = { "ecb", 1, ecbArgsArray };

static void 
ecbFunc(const iocshArgBuf *args) {

    int nargs = 0;
    for (int i=0 ; i < MAX_ECB_ARGUMENT_NO  ; ++i) {
        if (args[0].aval.av[i] == NULL)
            break;
        else
            nargs = nargs + 1;
    }

    char *ptr_argv[MAX_ECB_ARGUMENT_NO];
    for (int i = 0 ; i < nargs ; ++i) {
        ptr_argv[i] = args[0].aval.av[i];
    }

    ecb_run(nargs, ptr_argv);
}

static void 
ecbRegister(void) {
    iocshRegister (&ecbDef, ecbFunc);
}
epicsExportRegistrar(ecbRegister);
