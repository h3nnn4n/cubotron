TARGET = $(notdir $(CURDIR))
BUILDDIR = $(abspath $(CURDIR)/build)

TEST_TARGETS := $(basename $(foreach src,$(wildcard test/test_*.c), $(BUILDDIR)/$(src)))
TESTDIR = $(abspath $(CURDIR)/test)

OPTIONS =

INCLUDES = -Isrc \
           -Ideps/Unity/src \
           -Ideps/pcg-c/include

override CFLAGS += -Wall -Wextra -pedantic -std=gnu11 $(OPTIMIZATION) $(OPTIONS) $(INCLUDES)

OPTIMIZATION=-O0

LIBS =

BLACKLIST = -O0 -O1 -O2 -O3 -Os
LIBS := $(filter-out $(BLACKLIST), $(LIBS))

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  ECHOFLAGS = -e
  LDFLAGS = -lpcg_random -Wl,-Ldeps/Unity/build/,-Ldeps/pcg-c/src/
endif
ifeq ($(UNAME_S),Darwin)
  CFLAGS += -Wno-unused-command-line-argument
  LDFLAGS = -lpcg_random -Wl,-Ldeps/Unity/build/ -Wl,-Bstatic -Ldeps/pcg-c/src/
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

$(TEST_TARGETS): $(OBJS_NO_MAIN) $(OBJS_TEST)
	@echo $(ECHOFLAGS) "[LD]\t$<"
	$(CC) $(LDFLAGS) -o "$@" $@.o $(OBJS_NO_MAIN) $(LIBS)

$(BUILDDIR)/%.o: %.c
	@echo $(ECHOFLAGS) "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(TARGET): $(OBJS)
	@echo $(ECHOFLAGS) "[LD]\t$<"
	@$(CC) $(LDFLAGS) -o "$@" $^ $(LIBS)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)/src/"
	@rm -rf "$(BUILDDIR)/test/"
	@rm -rf "$(BUILDDIR)/deps/"
	@rm -f "$(TARGET).o"
	@rm -f "$(TARGET)"
