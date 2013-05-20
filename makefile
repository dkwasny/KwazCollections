# A makefile for compiling KwazCollections under both the
# clang and/or gnu compilers
#
# The only special part of this file is the generated compilation commands.
#
# To get around gxx/clang's restriction of disallowing the use of -o with
# multiple input files, I use make functions to generate a list of gxx/clang
# commands into a set of variables. 
# These variables are then referenced in each compilation task.
#
# -David Kwasny

# Build directories
BUILD_DIR := ./build
GCC_BUILD_DIR := $(BUILD_DIR)/gcc
GPP_BUILD_DIR := $(BUILD_DIR)/gpp
CLANG_BUILD_DIR := $(BUILD_DIR)/clang
CLANGPP_BUILD_DIR := $(BUILD_DIR)/clangpp

# Source directories
C_SOURCE_DIR := ./csrc
CPP_SOURCE_DIR := ./src

# Compilation commands
CLANG := clang -Wall -Wextra -Werror -pedantic-errors -c
CLANGPP := clang++ -Wall -Wextra -Werror -pedantic-errors -c
GCC := gcc -Wall -Wextra -Werror -pedantic-errors -c
GPP := g++ -Wall -Wextra -Werror -pedantic-errors -c

# Generated variables to assist in C compilation
C_SOURCE_FILES := $(wildcard $(C_SOURCE_DIR)/*.c)
C_OBJECT_NAMES := $(basename $(notdir $(C_SOURCE_FILES)))
GCC_COMPILE := $(foreach object, $(C_OBJECT_NAMES), $(GCC) -o $(GCC_BUILD_DIR)/$(object).o $(C_SOURCE_DIR)/$(object).c;)
CLANG_COMPILE := $(foreach object, $(C_OBJECT_NAMES), $(CLANG) -o $(CLANG_BUILD_DIR)/$(object).o $(C_SOURCE_DIR)/$(object).c;)

# Generated variables to assist in C++ compilation
CPP_SOURCE_FILES := $(wildcard $(CPP_SOURCE_DIR)/*.cpp)
CPP_OBJECT_NAMES := $(basename $(notdir $(CPP_SOURCE_FILES)))
GPP_COMPILE := $(foreach object, $(CPP_OBJECT_NAMES), $(GPP) -o $(GPP_BUILD_DIR)/$(object).o $(CPP_SOURCE_DIR)/$(object).cpp;)
CLANGPP_COMPILE := $(foreach object, $(CPP_OBJECT_NAMES), $(CLANGPP) -o $(CLANGPP_BUILD_DIR)/$(object).o $(CPP_SOURCE_DIR)/$(object).cpp;)

.PHONY: all
all: gnu clang

.PHONY: c
c: gccbuild clangbuild

.PHONY: cpp
cpp: gppbuild clangppbuild

.PHONY: gnu
gnu: gccbuild gppbuild

.PHONY: clang
clang: clangbuild clangppbuild

.PHONY: gccbuild
gccbuild: $(C_SOURCE_FILES) | $(GCC_BUILD_DIR)
	$(GCC_COMPILE)

.PHONY: gppbuild
gppbuild: $(CPP_SOURCE_FILES) | $(GPP_BUILD_DIR)
	$(GPP_COMPILE)

.PHONY: clangbuild
clangbuild: $(C_SOURCE_FILES) | $(CLANG_BUILD_DIR)
	$(CLANG_COMPILE)

.PHONY: clangppbuild
clangppbuild: $(CPP_SOURCE_FILES) | $(CLANGPP_BUILD_DIR)
	$(CLANGPP_COMPILE)

# TODO: Maybe make an archive target that brings all the .o files
# into 1 lib file.  One per lang/compiler of course...

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# Build directory structure creation targets
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(GCC_BUILD_DIR): | $(BUILD_DIR)
	mkdir $(GCC_BUILD_DIR)

$(GPP_BUILD_DIR): | $(BUILD_DIR)
	mkdir $(GPP_BUILD_DIR)

$(CLANG_BUILD_DIR): | $(BUILD_DIR)
	mkdir $(CLANG_BUILD_DIR)

$(CLANGPP_BUILD_DIR): | $(BUILD_DIR)
	mkdir $(CLANGPP_BUILD_DIR)
