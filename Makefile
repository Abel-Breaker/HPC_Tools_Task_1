# Prefix for silent compilation
Q ?= @

# Default compile mode
MODE ?= debug

# Executable name
TARGET ?= build/program

# Compiler (default gcc)
CC ?= gcc

ifeq ($(findstring gcc,$(CC)),)
ifeq ($(findstring icc,$(CC)),)
ifeq ($(findstring icx,$(CC)),)

$(error Unsupported compiler '$(CC)'. Valid compilers: $(VALID_COMPILERS))

endif
endif
endif

# Number of processors
NPROC ?= $(shell nproc)

# Search all .c files in src/.
SRC := $(shell find ./src/ -type f -name "*.c")

# Objects in directory build/$(MODE), so we can reutilise .o for each mode
OBJDIR := build/$(MODE)
OBJ := $(patsubst ./%.c,$(OBJDIR)/%.o,$(SRC))


# Common flags
CFLAGS := -std=c11 -MMD -MP 

LDLIBS  := 

LDFLAGS :=

DEBUG_COMMON_FLAGS := -O0 -g3 -Wall -Wextra  -Wshadow -Wformat=2 \
-Wconversion -Wsign-conversion -Wuninitialized \
-Wunused -Wpointer-arith -Wcast-qual -Wstrict-prototypes \
-fno-omit-frame-pointer -fstack-protector-strong \
-Wundef  -Wwrite-strings -fno-common -Wno-unknown-pragmas \
-Wswitch-default -Wmissing-prototypes -Wmissing-declarations \
-Wswitch-enum -Wdeprecated -Winit-self -Wvla

DEBUG_FLAGS_GCC := -Wpedantic -Wnull-dereference -Wdouble-promotion \
			-Wstack-protector -fstack-clash-protection \
			-Wshift-negative-value -Wshift-overflow -Wcast-align -fanalyzer

# Release flags
RELEASE_COMMON_FLAGS := -O2 -march=native


ifeq ($(MODE),debug)
CFLAGS += $(DEBUG_COMMON_FLAGS)
ifneq ($(findstring gcc,$(CC)),)
CFLAGS += $(DEBUG_FLAGS_GCC)
endif
else ifeq ($(MODE),release)
CFLAGS += $(RELEASE_COMMON_FLAGS)
else
$(error Invalid MODE '$(MODE)'. Valid modes are: debug, release)
endif

# Intel compiler links his own optimized math library
ifneq ($(findstring gcc,$(CC)),)
CFLAGS += -lm
LDLIBS += -lm
endif

# Rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(Q)$(CC) $(LDFLAGS) $(OBJ) -o $@ $(LDLIBS)

$(OBJDIR)/%.o: %.c
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

rebuild:
	$(Q)$(MAKE) --no-print-directory -B all

clean:
	$(Q)rm -rf $(OBJDIR) $(TARGET)

fclean:
	$(Q)rm -rf build


help:
	@echo "Use: make [target] [MODE=mode] [CC=compilator] [TARGET=executable_name] [NPROC=number_of_processors]"
	@echo ""
	@echo "Target (default: all)"
	@echo "  all                Compiles $(TARGET) in the specified mode"
	@echo "  clean              Deletes object files of the specified mode and the binary"
	@echo "  fclean             Deletes the entire build/ directory (all modes) and the binary"
	@echo "  rebuild            Recompile the entire build"
	@echo "  help               Displays this help message"
	@echo ""
	@echo "MODE (default: $(MODE))"
	@echo "  debug          No optimizations + Debuggin info"
	@echo "  release        Optimizations (choose manually)"
	@echo ""
	@echo "CC (default: gcc)"
	@echo "  Compiler to use, e.g. make CC=clang"
	@echo ""
	@echo "TARGET (default: program)"
	@echo "  Executable name"
	@echo ""
	@echo "NPROC (default: Number of proccesors in your system)"
	@echo "  Specifies the number of processes for parallelizing the static analyzers tools."
	@echo ""


.PHONY: all clean fclean rebuild help
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables

# Avoid recompiling innecessary files
-include $(OBJ:.o=.d)