CXXFLAGS +=-I../vendor/nlohmann -I../vendor/inja -I../vendor/rapidyaml
CXXFLAGS +=-O3

SRC := $(wildcard **.cc)
SRC_EXE:=$(filter-out $(wildcard *_test.cc), $(SRC))

OBJS=$(SRC_EXE:.cc=.o)
OBJS_TEST=$(SRC:.cc=.o)

ecb: $(OBJS)

test_ecb: $(OBJS_TEST)

clean:
	rm -rf *.o
	rm -rf ecb
