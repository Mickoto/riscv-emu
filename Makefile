SOURCES=$(wildcard src/*.cpp)
OBJS=$(patsubst src/%.cpp,build/%.o,$(SOURCES))
TESTSRCS=$(wildcard test/*.c)
TESTS=$(patsubst test/%.c,build/test/%,$(TESTSRCS))

CFLAGS =-Wall -ggdb -Werror -std=c++23
RISCVC =riscv32-unknown-elf-gcc
TESTFLAGS=-march=rv32i -mabi=ilp32 -nostdlib -fno-builtin

riscv-emu: $(OBJS)
	$(CXX) $(CFLAGS) -o riscv-emu $(OBJS)

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CFLAGS) -c -o $@ $<

tests: $(TESTS)
	for test in build/test/*; do ./riscv-emu $$test; done

build/test/%: test/%.c
	mkdir -p build/test
	$(RISCVC) $(TESTFLAGS) -o $@ $<
