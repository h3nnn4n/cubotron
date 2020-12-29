TARGET = $(notdir $(CURDIR))
BUILDDIR = $(abspath $(CURDIR)/build)

OPTIONS = -DIMGUI_IMPL_API="extern \"C\"" \

INCLUDES = -Isrc

LINKS = -Ldeps/

override CFLAGS += -Wall -Wextra -pedantic -std=gnu11 $(OPTIMIZATION) $(OPTIONS) $(INCLUDES)

OPTIMIZATION=-O0

LDFLAGS =

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

SOURCES := $(C_FILES:.c=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))

all: build

build: $(TARGET)

rebuild: clean $(TARGET)

run: $(TARGET)
	$(CURDIR)/$(TARGET)

gdb: $(TARGET)
	gdb $(CURDIR)/$(TARGET)

$(BUILDDIR)/%.o: %.c
	@echo $(ECHOFLAGS) "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(TARGET).o: $(OBJS) $(LDSCRIPT)
	@echo $(ECHOFLAGS) "[LD]\t$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

$(TARGET): $(OBJS) $(LDSCRIPT)
	@echo $(ECHOFLAGS) "[LD]\t$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

-include $(OBJS:.o=.d)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)/src/"
	@rm -f "$(TARGET).o"

superclean:
	@echo Activating clean slate protocol
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(TARGET).o"
