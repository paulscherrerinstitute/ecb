# ECB - ECMC configuration builder

VERSION = 1.0.0

CC = clang++
CFLAGS = -std=c++17 -Wall -Werror -O3
CFLAGS_TEST = -g -O0 -std=c++17 -Wall -Werror
LIBS_TEST = -lpthread -lgtest -lgtest_main

EXE = ecb
EXE_TEST = test_ecb

SRC := $(wildcard src/**.cc)
HEADER := $(wildcard src/**.h)

# sources for program
SRC_EXE := $(filter-out $(wildcard src/*_test.cc), $(SRC))

# sources for test
SRC_TEST := $(filter-out src/ecb.cc, $(SRC_EXE))
TESTS :=$(wildcard src/*_test.cc)

INCLUDE := -Isrc -Ivendor/nlohmann -Ivendor/inja -Ivendor/rapidyaml

# build info
BUILD_NUMBER = $(shell git rev-list --count master)
BUILD_DIRTY  = $(shell [ -n "$$(git status --porcelain)" ] && echo "-dirty")
BUILD_HASH = $(shell git rev-parse HEAD)
BUILD_DATE = $(shell date -Iseconds)
export BUILD_INFO = -DMAKEFILE_VERSION="\"$(VERSION)\"" -DMAKEFILE_BUILD_NUMBER="\"$(BUILD_NUMBER)\"" -DMAKEFILE_BUILD_DIRTY="\"$(BUILD_DIRTY)\"" -DMAKEFILE_BUILD_HASH="\"$(BUILD_HASH)\"" -DMAKEFILE_BUILD_DATE="\"$(BUILD_DATE)\""

ASTYLE := --style=bsd --indent=spaces=4 --indent-switches --break-blocks --pad-oper --pad-comma --pad-header --unpad-paren --align-pointer=type --align-reference=type --max-code-length=100 --break-closing-braces --convert-tabs --remove-braces --suffix=none --indent-after-parens

all: $(SRC)
	$(CC) -DJSON_DIAGNOSTICS=1 $(CFLAGS) $(BUILD_INFO) $(INCLUDE) $(SRC_EXE) -o $(EXE)

debug: $(SRC)
	$(CC) -DJSON_DIAGNOSTICS=1 $(CFLAGS_TEST) $(BUILD_INFO) $(INCLUDE) $(SRC_EXE) -o $(EXE)

test:
	$(CC) $(CFLAGS_TEST) $(BUILD_INFO) $(LIBS_TEST) $(INCLUDE) $(SRC_TEST) $(TESTS) -o $(EXE_TEST)

format:
	astyle $(ASTYLE) $(SRC) $(HEADER)

clean:
	rm -rf $(EXE_TEST)
	rm -rf $(EXE)
