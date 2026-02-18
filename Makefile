.DEFAULT_GOAL := build

TARGET = cubotron
BUILDDIR = $(abspath $(CURDIR)/build)

TEST_TARGETS := $(basename $(foreach src,$(wildcard test/test_*.c), $(BUILDDIR)/$(src)))
TESTDIR = $(abspath $(CURDIR)/test)

OPTIONS =

INCLUDES = -Isrc \
           -Ideps/Unity/src \
           -Ideps/pcg-c/include \
           -Ideps/pcg-c/extras

OPTIMIZATION=-O3

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  ECHOFLAGS = -e
  LDFLAGS = -lpcg_random -lm -Wl,-Ldeps/Unity/build/,-Ldeps/pcg-c/src/
endif
ifeq ($(UNAME_S),Darwin)
  CFLAGS += -Wno-unused-command-line-argument -Wno-strict-prototypes
  LDFLAGS = -lpcg_random -lm -Wl,-Ldeps/pcg-c/src/
endif

override CFLAGS += -Wall -Wextra -pedantic -Werror -std=gnu11 $(OPTIMIZATION) $(OPTIONS) $(INCLUDES)

CC = gcc

C_FILES := $(wildcard src/*.c) \
           $(wildcard deps/pcg-c/extras/*.c)
C_FILES_TEST := $(wildcard test/*.c)
C_FILES_TEST_DEPS := $(wildcard deps/Unity/src/*.c)

SOURCES := $(C_FILES:.c=.o)
SOURCES_TEST := $(C_FILES_TEST:.c=.o)
SOURCES_TEST_DEPS := $(C_FILES_TEST_DEPS:.c=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))
OBJS_TEST := $(foreach src,$(SOURCES_TEST), $(BUILDDIR)/$(src))
OBJS_NO_MAIN := $(filter-out %main.o, $(OBJS)) \
                $(C_FILES_TEST_DEPS)

.PHONY: test
.PHONY: clean
.PHONY: cpplint
.PHONY: cppcheck
.PHONY: clang-format
.PHONY: format
.PHONY: lint-all
.PHONY: test-ci
.PHONY: solve
.PHONY: solve-samples
.PHONY: benchmark
.PHONY: heapcheck-solve
.PHONY: heapcheck-solve-blacklist
.PHONY: heapcheck-benchmark
.PHONY: ci

all: build

build: pcg pcg_full $(TARGET)

debug: debug_prepare build

debug_prepare:
	$(eval OPTIMIZATION=-g -pg -O0)

gperftools: gperftools_prepare build

gperftools_prepare:
	$(eval OPTIMIZATION=-DWITHGPERFTOOLS -lprofiler -ltcmalloc -g -pg -O2 -DNDEBUG -fno-inline-functions -fno-inline-functions-called-once -fno-optimize-sibling-calls)

callgrind: callgrind_prepare build

callgrind_prepare:
	$(eval OPTIMIZATION=-g -O2 -DNDEBUG -fno-inline-functions -fno-inline-functions-called-once -fno-optimize-sibling-calls -fno-default-inline -fno-inline)

rebuild: clean $(TARGET)

retest: clean test

run: $(TARGET)
	$(CURDIR)/$(TARGET)

gdb: clean debug_prepare $(TARGET)
	gdb $(CURDIR)/$(TARGET)

test: pcg $(TEST_TARGETS)
	@for var in $(TEST_TARGETS); do \
		echo $(ECHOFLAGS) "[RUN]\t$$var"; \
		$$var || exit $$?; \
	done; \
	echo $(ECHOFLAGS) "Everything in order"

test-%: $(BUILDDIR)/test/test_% pcg
	@echo $(ECHOFLAGS) "[RUN]\t$(BUILDDIR)/test/test_$*"
	@$(BUILDDIR)/test/test_$*

pcg: $(BUILDDIR)/.pcg_core

$(BUILDDIR)/.pcg_core:
	@echo $(ECHOFLAGS) "[CC]\tpcg core"
	@$(MAKE) -s -C deps/pcg-c/src/
	@mkdir -p $(BUILDDIR)
	@touch $@

pcg_full: $(BUILDDIR)/.pcg_full

$(BUILDDIR)/.pcg_full:
	@echo $(ECHOFLAGS) "[CC]\tpcg full"
	@$(MAKE) -s -C deps/pcg-c/
	@mkdir -p $(BUILDDIR)
	@touch $@

pcg_clean:
	@$(MAKE) clean -C deps/pcg-c/src/ > /dev/null
	@rm -f $(BUILDDIR)/.pcg_core $(BUILDDIR)/.pcg_full

$(TEST_TARGETS): $(OBJS_NO_MAIN) $(OBJS_TEST)
	@echo $(ECHOFLAGS) "[LD]\t$@"
	@$(CC) -o "$@" $@.o $(OBJS_NO_MAIN) $(LDFLAGS) -g -pg -O0

$(BUILDDIR)/%.o: %.c
	@echo $(ECHOFLAGS) "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(TARGET): $(OBJS)
	@echo $(ECHOFLAGS) "[LD]\t$@"
	@$(CC) -o "$@" $^ $(LDFLAGS) $(OPTIMIZATION)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)/src/"
	@rm -rf "$(BUILDDIR)/test/"
	@rm -rf "$(BUILDDIR)/deps/"
	@rm -f "$(TARGET).o"
	@rm -f "$(TARGET)"

# Linters
cpplint:
	@.github/scripts/ci.sh cpplint

cppcheck:
	@.github/scripts/ci.sh cppcheck

clang-format:
	@.github/scripts/ci.sh clang-format

format:
	@.github/scripts/ci.sh format

lint-all:
	@.github/scripts/ci.sh lint-all

# Tests
test-ci:
	@.github/scripts/ci.sh test

solve:
	@.github/scripts/ci.sh solve

solve-samples:
	@.github/scripts/ci.sh solve-samples

benchmark:
	@.github/scripts/ci.sh benchmark

# Memory checks
heapcheck-solve:
	@.github/scripts/ci.sh heapcheck-solve

heapcheck-solve-blacklist:
	@.github/scripts/ci.sh heapcheck-solve-blacklist

heapcheck-benchmark:
	@.github/scripts/ci.sh heapcheck-benchmark

# Run all CI checks
ci:
	@.github/scripts/ci.sh all
