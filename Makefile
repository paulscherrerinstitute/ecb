VERSION = 1.4.0

ARCH ?= rhel

export CXXFLAGS = -std=c++17 -Wall -Werror $(BUILD_INFO)

ifeq ($(ARCH),rhel)
export CC := clang++
export CXX := clang++
endif

# ifeq ($(ARCH),deb10)
# export CC := /opt/xgcc/gcc-8.3.0-deb10/bin/x86_64-deb10-linux-gnu-c++
# export CXX := /opt/xgcc/gcc-8.3.0-deb10/bin/x86_64-deb10-linux-gnu-c++
# endif
#
# ifeq ($(ARCH),deb12)
# export CC := /opt/xgcc/gcc-12.2.0-deb12/bin/x86_64-deb12-linux-gnu-c++
# export CXX := /opt/xgcc/gcc-12.2.0-deb12/bin/x86_64-deb12-linux-gnu-c++
# export LDFLAGS := -static
# endif

# export build information
BUILD_NUMBER = $(shell git rev-list --count main)
BUILD_DIRTY  = $(shell [ -n "$$(git status --porcelain)" ] && echo "-dirty")
BUILD_HASH = $(shell git rev-parse HEAD)
BUILD_DATE = $(shell date -Iseconds)
export BUILD_INFO = -DMAKEFILE_VERSION="\"$(VERSION)\"" -DMAKEFILE_BUILD_NUMBER="\"$(BUILD_NUMBER)\"" -DMAKEFILE_BUILD_DIRTY="\"$(BUILD_DIRTY)\"" -DMAKEFILE_BUILD_HASH="\"$(BUILD_HASH)\"" -DMAKEFILE_BUILD_DATE="\"$(BUILD_DATE)\""


all: ecb

ecb:
	$(MAKE) -C src -f Makefile
	mv src/ecb ./bin/ecb

debug:
	$(MAKE) -C src -f Makefile.DEBUG
	mv src/ecb ./bin/ecb_$(ARCH)_debug

test:
	$(MAKE) -C src -f Makefile.TEST test_ecb
	mv src/test_ecb ./bin/ecb_$(ARCH)_test

checkstyle:
	astyle --style=bsd --indent=spaces=4 --indent-switches --break-blocks --pad-oper --pad-comma --pad-header --unpad-paren --align-pointer=type --align-reference=type --max-code-length=100 --break-closing-braces --convert-tabs --remove-braces --suffix=none --indent-after-parens ./src/*.{cc,h}

clean:
	$(MAKE) -C src -f Makefile clean
	rm -rf ./bin/ecb_rhel
	rm -rf ./bin/ecb_deb10
	rm -rf ./bin/ecb_deb12
	rm -rf ./bin/ecb_rhel_debug
	rm -rf ./bin/ecb_deb10_debug
	rm -rf ./bin/ecb_deb12_debug
	rm -rf ./bin/ecb_rhel_test
	rm -rf ./bin/ecb_deb10_test
	rm -rf ./bin/ecb_deb12_test
