TARGET = $(notdir $(CURDIR))
BUILDDIR = $(abspath $(CURDIR)/build)

TEST_TARGET = $(BUILDDIR)/test/runner
TESTDIR = $(abspath $(CURDIR)/test)

OPTIONS =

INCLUDES = -Isrc \
           -Ideps/Unity/src

LINKS = -Ldeps/

override CFLAGS += -Wall -Wextra -pedantic -std=gnu11 $(OPTIMIZATION) $(OPTIONS) $(INCLUDES)

OPTIMIZATION=-O0

LDFLAGS = -Wl,-Ldeps/Unity/build/

#LIBS = -lunity
LIBS =

BLACKLIST = -O0 -O1 -O2 -O3 -Os
LIBS := $(filter-out $(BLACKLIST), $(LIBS))

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  ECHOFLAGS = -e
endif
ifeq ($(UNAME_S),Darwin)
  CFLAGS += -Wno-unused-command-line-argument
endif

CC = gcc

C_FILES := $(wildcard src/*.c)
C_FILES_TEST := $(wildcard test/*.c)

SOURCES := $(C_FILES:.c=.o)
SOURCES_TEST := $(C_FILES_TEST:.c=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))
OBJS_TEST := $(foreach src,$(SOURCES_TEST), $(BUILDDIR)/$(src)) \
             $(filter-out %main.o, $(OBJS))

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

test: $(TEST_TARGET)
	$(TEST_TARGET)

$(TEST_TARGET): $(OBJS_TEST)
	@echo $(ECHOFLAGS) "[LD]\t$<"
	@$(CC) $(LDFLAGS) -o "$@" $^ $(LIBS)

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
