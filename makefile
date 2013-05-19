# A hideous makefile for compiling KwazCollections under both the
# clang and/or gnu compilers
#
# I am really sad that I have to keep track of every source and output
# file name, but that appears to be the way c compilers work.
#
# -David Kwasny

# Build directories
BUILD_DIR := ./build
GCC_BUILD_DIR := $(BUILD_DIR)/gcc
GPP_BUILD_DIR := $(BUILD_DIR)/gpp
CLANG_BUILD_DIR := $(BUILD_DIR)/clang
CLANGPP_BUILD_DIR := $(BUILD_DIR)/clangpp

# Class/object name constants...eww
ARRAY_LIST := ArrayList

# C source and build files...eww
C_SOURCE_DIR := ./csrc
C_ARRAY_LIST := $(C_SOURCE_DIR)/$(ARRAY_LIST).c
GCC_ARRAY_LIST_O := $(GCC_BUILD_DIR)/$(ARRAY_LIST).o
CLANG_ARRAY_LIST_O := $(CLANG_BUILD_DIR)/$(ARRAY_LIST).o

# C++ source and build files...eww
CPP_SOURCE_DIR := ./src
CPP_ARRAY_LIST := $(CPP_SOURCE_DIR)/$(ARRAY_LIST).cpp
GPP_ARRAY_LIST_O := $(GPP_BUILD_DIR)/$(ARRAY_LIST).o
CLANGPP_ARRAY_LIST_O := $(CLANGPP_BUILD_DIR)/$(ARRAY_LIST).o

# Compilation commands
CLANG := clang -Wall
CLANGPP := clang++ -Wall
GCC := gcc -Wall
GPP := g++ -Wall

.PHONY: all
all: gnu clang

.PHONY: gnu
gnu: gccbuild gppbuild

.PHONY: clang
clang: clangbuild clangppbuild

.PHONY: gccbuild
gccbuild: | $(GCC_BUILD_DIR)
	$(GCC) $(C_ARRAY_LIST) -c -o $(GCC_ARRAY_LIST_O)

.PHONY: gppbuild
gppbuild: | $(GPP_BUILD_DIR)
	$(GPP) $(CPP_ARRAY_LIST) -c -o $(GPP_ARRAY_LIST_O)

.PHONY: clangbuild
clangbuild: | $(CLANG_BUILD_DIR)
	$(CLANG) $(C_ARRAY_LIST) -c -o $(CLANG_ARRAY_LIST_O)

.PHONY: clangppbuild
clangppbuild: | $(CLANGPP_BUILD_DIR)
	$(CLANGPP) $(CPP_ARRAY_LIST) -c -o $(CLANGPP_ARRAY_LIST_O)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

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
