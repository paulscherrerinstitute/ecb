ARCH ?= default

export CXXFLAGS = -std=c++17 -Wall -Werror

include src/Makefile.COMMON

ifeq ($(ARCH),default)
export CC := g++
export CXX := g++
endif

ifeq ($(ARCH),deb10)
export CC := /opt/xgcc/gcc-8.3.0-deb10/bin/x86_64-deb10-linux-gnu-c++
export CXX := /opt/xgcc/gcc-8.3.0-deb10/bin/x86_64-deb10-linux-gnu-c++
endif

ifeq ($(ARCH),deb12)
export CC := /opt/xgcc/gcc-12.2.0-deb12/bin/x86_64-deb12-linux-gnu-c++
export CXX := /opt/xgcc/gcc-12.2.0-deb12/bin/x86_64-deb12-linux-gnu-c++
endif

all: ecb

ecb:
	$(MAKE) -C src -f Makefile
	mv src/ecb ./bin/ecb

debug:
	$(MAKE) -C src -f Makefile.DEBUG
	mv src/ecb ./bin/ecb_$(ARCH)_debug

test:
	$(MAKE) -C src -f Makefile.TEST test_ecb
	mv src/test_ecb ./bin/ecb_test

checkstyle:
	astyle --style=bsd --indent=spaces=4 --indent-switches --break-blocks --pad-oper --pad-comma --pad-header --unpad-paren --align-pointer=type --align-reference=type --max-code-length=100 --break-closing-braces --convert-tabs --remove-braces --suffix=none --indent-after-parens ./src/*.{cc,h}

clean:
	$(MAKE) -C src -f Makefile clean
	rm -rf ./bin/ecb
	rm -rf ./bin/ecb_debug
	rm -rf ./bin/ecb_test
