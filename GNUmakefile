include /ioc/tools/driver.makefile

MODULE=ecb

BUILDCLASSES = Linux

EXCLUDE_VERSIONS = 3 7.0.5 7.0.6 7.0.7

ARCH_FILTER=deb% RHEL%

USR_CPPFLAGS += -O3
-include ../src/Makefile.COMMON

USR_INCLUDES += -I../vendor
USR_INCLUDES += -I../vendor/inja
USR_INCLUDES += -I../vendor/rapidyaml
USR_SYS_LIBS += stdc++fs

SOURCES += $(filter-out $(wildcard src/*_test.cc), $(wildcard src/*.cc))

SCRIPTS += startup.cmd

DBDS    += $(wildcard ./src/*.dbd)
