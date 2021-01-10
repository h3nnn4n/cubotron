.DEFAULT_GOAL := test

TARGET = $(notdir $(CURDIR))
BUILDDIR = $(abspath $(CURDIR)/build)

TEST_TARGETS := $(basename $(foreach src,$(wildcard test/test_*.c), $(BUILDDIR)/$(src)))
TESTDIR = $(abspath $(CURDIR)/test)

OPTIONS =

INCLUDES = -Isrc \
           -Ideps/Unity/src \
           -Ideps/pcg-c/include

OPTIMIZATION=-O3
#OPTIMIZATION=-O0 -g

override CFLAGS += -Wall -Wextra -pedantic -std=gnu11 $(OPTIMIZATION) $(OPTIONS) $(INCLUDES)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  ECHOFLAGS = -e
  LDFLAGS = -lpcg_random -Wl,-Ldeps/Unity/build/,-Ldeps/pcg-c/src/
endif
ifeq ($(UNAME_S),Darwin)
  CFLAGS += -Wno-unused-command-line-argument
  LDFLAGS = -lpcg_random -Wl,-Ldeps/Unity/build/,-Ldeps/pcg-c/src/
endif

CC = gcc

C_FILES := $(wildcard src/*.c)
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

all: build

build: $(TARGET)

rebuild: clean $(TARGET)

retest: clean test

run: $(TARGET)
	$(CURDIR)/$(TARGET)

gdb: $(TARGET)
	gdb $(CURDIR)/$(TARGET)

test: pcg $(TEST_TARGETS)
	$(foreach var,$(TEST_TARGETS),$(var) && ) echo $(ECHOFLAGS) "Everything in order"

pcg:
	$(MAKE) -C deps/pcg-c/src/

pcg_clean:
	@$(MAKE) clean -C deps/pcg-c/src/ > /dev/null

$(TEST_TARGETS): $(OBJS_NO_MAIN) $(OBJS_TEST)
	@echo $(ECHOFLAGS) "[LD]\t$<"
	$(CC) -o "$@" $@.o $(OBJS_NO_MAIN) $(LDFLAGS) $(OPTIMIZATION)

$(BUILDDIR)/%.o: %.c
	@echo $(ECHOFLAGS) "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(TARGET): $(OBJS)
	@echo $(ECHOFLAGS) "[LD]\t$<"
	@$(CC) -o "$@" $^ $(LDFLAGS) $(OPTIMIZATION)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)/src/"
	@rm -rf "$(BUILDDIR)/test/"
	@rm -rf "$(BUILDDIR)/deps/"
	@rm -f "$(TARGET).o"
	@rm -f "$(TARGET)"
