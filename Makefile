.DEFAULT_GOAL := build

TARGET = $(notdir $(CURDIR))
BUILDDIR = $(abspath $(CURDIR)/build)

TEST_TARGETS := $(basename $(foreach src,$(wildcard test/test_*.c), $(BUILDDIR)/$(src)))
TESTDIR = $(abspath $(CURDIR)/test)

OPTIONS =

INCLUDES = -Isrc \
           -Ideps/Unity/src \
           -Ideps/pcg-c/include \
           -Ideps/pcg-c/extras

OPTIMIZATION=-O3
SIZE_OPTIMIZATION=-Os -ffunction-sections -fdata-sections -Wl,--gc-sections -s
SPEED_OPTIMIZATION=-O3 -march=native -mtune=native -flto -ffast-math -funroll-loops -fomit-frame-pointer -fno-stack-protector -DNDEBUG -fopenmp -ftree-vectorize
ULTRA_SPEED_OPTIMIZATION=-Ofast -march=native -mtune=native -flto -ffast-math -funroll-loops -fomit-frame-pointer -fno-stack-protector -finline-functions -finline-limit=1000 -fno-strict-aliasing -DNDEBUG -fopenmp -ftree-vectorize -fwhole-program -fno-common
PROFILE_GUIDED_OPTIMIZATION=-O3 -march=native -mtune=native -flto -ffast-math -funroll-loops -fomit-frame-pointer -fno-stack-protector -fprofile-use -fprofile-correction -DNDEBUG -fopenmp -ftree-vectorize

override CFLAGS += -Wall -Wextra -pedantic -std=gnu11 $(OPTIMIZATION) $(OPTIONS) $(INCLUDES)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  ECHOFLAGS = -e
  LDFLAGS = -lpcg_random -Wl,-Ldeps/Unity/build/,-Ldeps/pcg-c/src/
  SPEED_LDFLAGS = -lpcg_random -Wl,-Ldeps/Unity/build/,-Ldeps/pcg-c/src/ -Wl,--as-needed -Wl,--strip-all -Wl,-z,relro -Wl,-z,now
  ULTRA_LDFLAGS = -lpcg_random -Wl,-Ldeps/Unity/build/,-Ldeps/pcg-c/src/ -Wl,--as-needed -Wl,--strip-all -Wl,-z,relro -Wl,-z,now -Wl,--gc-sections -Wl,--build-id=none
endif
ifeq ($(UNAME_S),Darwin)
  CFLAGS += -Wno-unused-command-line-argument
  LDFLAGS = -lpcg_random -Wl,-Ldeps/pcg-c/src/
  SPEED_LDFLAGS = -lpcg_random -Wl,-Ldeps/pcg-c/src/ -Wl,-dead_strip -Wl,-S
  ULTRA_LDFLAGS = -lpcg_random -Wl,-Ldeps/pcg-c/src/ -Wl,-dead_strip -Wl,-S -Wl,-x
endif

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

all: build

build: pcg pcg_full $(TARGET)

size: pcg pcg_full $(TARGET)_size

speed: pcg pcg_full $(TARGET)_speed

ultra: pcg pcg_full $(TARGET)_ultra

pgo: pgo_instrument pgo_optimize

pgo_instrument: pcg pcg_full $(TARGET)_pgo_instrument

pgo_optimize: pcg pcg_full $(TARGET)_pgo_optimize

benchmark: speed
	@echo $(ECHOFLAGS) "Running benchmark on speed-optimized build..."
	@./$(TARGET)_speed

benchmark_ultra: ultra
	@echo $(ECHOFLAGS) "Running benchmark on ultra-optimized build..."
	@./$(TARGET)_ultra

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
	$(foreach var,$(TEST_TARGETS),$(var) && ) echo $(ECHOFLAGS) "Everything in order"

pcg:
	@echo $(ECHOFLAGS) "[CC]\tpcg core"
	@$(MAKE) -s -C deps/pcg-c/src/

pcg_full:
	@echo $(ECHOFLAGS) "[CC]\tpcg full"
	@$(MAKE) -s -C deps/pcg-c/

pcg_clean:
	@$(MAKE) clean -C deps/pcg-c/src/ > /dev/null

$(TEST_TARGETS): $(OBJS_NO_MAIN) $(OBJS_TEST)
	@echo $(ECHOFLAGS) "[LD]\t$@"
	@$(CC) -o "$@" $@.o $(OBJS_NO_MAIN) $(LDFLAGS) $(OPTIMIZATION)

$(BUILDDIR)/%.o: %.c
	@echo $(ECHOFLAGS) "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(TARGET): $(OBJS)
	@echo $(ECHOFLAGS) "[LD]\t$@"
	@$(CC) -o "$@" $^ $(LDFLAGS) $(OPTIMIZATION)

$(TARGET)_size: $(OBJS)
	@echo $(ECHOFLAGS) "[LD]\t$@ (size optimized)"
	@$(CC) -o "$@" $^ $(LDFLAGS) $(SIZE_OPTIMIZATION)

$(TARGET)_speed: $(OBJS)
	@echo $(ECHOFLAGS) "[LD]\t$@ (speed optimized)"
	@$(CC) -o "$@" $^ $(SPEED_LDFLAGS) $(SPEED_OPTIMIZATION)

$(TARGET)_ultra: $(OBJS)
	@echo $(ECHOFLAGS) "[LD]\t$@ (ultra speed optimized)"
	@$(CC) -o "$@" $^ $(ULTRA_LDFLAGS) $(ULTRA_SPEED_OPTIMIZATION)

$(TARGET)_pgo_instrument: $(OBJS)
	@echo $(ECHOFLAGS) "[LD]\t$@ (PGO instrumented)"
	@$(CC) -o "$@" $^ $(LDFLAGS) -O2 -march=native -mtune=native -flto -fprofile-generate -DNDEBUG

$(TARGET)_pgo_optimize: $(OBJS)
	@echo $(ECHOFLAGS) "[LD]\t$@ (PGO optimized)"
	@$(CC) -o "$@" $^ $(LDFLAGS) $(PROFILE_GUIDED_OPTIMIZATION)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)/src/"
	@rm -rf "$(BUILDDIR)/test/"
	@rm -rf "$(BUILDDIR)/deps/"
	@rm -f "$(TARGET).o"
	@rm -f "$(TARGET)" "$(TARGET)_size" "$(TARGET)_speed" "$(TARGET)_ultra" "$(TARGET)_pgo_instrument" "$(TARGET)_pgo_optimize"
	@rm -f *.gcda *.gcno
