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
BUILD_DIR := build
GCC_BUILD_DIR := $(BUILD_DIR)/gcc
CLANG_BUILD_DIR := $(BUILD_DIR)/clang

# Test executables
TEST_EXEC_NAME := tests.run
GCC_TEST_EXEC := $(GCC_BUILD_DIR)/$(TEST_EXEC_NAME)
CLANG_TEST_EXEC := $(CLANG_BUILD_DIR)/$(TEST_EXEC_NAME)

# KwazCollections C library info
C_LIB_FILENAME := libckwazcoll.a
GCC_LIB := $(GCC_BUILD_DIR)/$(C_LIB_FILENAME)
CLANG_LIB := $(CLANG_BUILD_DIR)/$(C_LIB_FILENAME)

# Source directories
C_SOURCE_DIR := src
C_INCLUDE_DIR := include

# Test source directories and suites
C_TEST_DIR := test
C_TEST_SUITE := $(C_TEST_DIR)/TestSuite.cpp

# Google Test stuff
GTEST_BUILD_DIR := $(BUILD_DIR)/gtest
GTEST_DIR := ./lib/gtest-1.7.0
GTEST_INCLUDE_DIR := $(GTEST_DIR)/include
GTEST_LIB := $(GTEST_BUILD_DIR)/libgtest.a

# Compilation commands
CLANG := clang -Wall -Wextra -Werror -pedantic-errors -ansi -I $(C_INCLUDE_DIR) -c
GCC := gcc -Wall -Wextra -Werror -pedantic-errors -ansi -I $(C_INCLUDE_DIR) -c

VALGRIND := valgrind --tool=memcheck --leak-check=full --error-exitcode=1

# Special compilation commands used to compile test code.
# I do not want to have to worry about my test code and google test
# passing the goofy ansi/pedantic crap I set for the main source.
GPP_TEST_COMPILE := g++ -Wall -Wextra -Werror
CLANGPP_TEST_COMPILE := clang++ -Wall -Wextra -Werror
GTEST_COMPILE := g++

# Generated variables to assist in C compilation
C_SOURCE_FILES := $(wildcard $(C_SOURCE_DIR)/*.c)
C_OBJECT_NAMES := $(basename $(notdir $(C_SOURCE_FILES)))
GCC_COMPILE := $(foreach object, $(C_OBJECT_NAMES), $(GCC) -o $(GCC_BUILD_DIR)/$(object).o $(C_SOURCE_DIR)/$(object).c;)
CLANG_COMPILE := $(foreach object, $(C_OBJECT_NAMES), $(CLANG) -o $(CLANG_BUILD_DIR)/$(object).o $(C_SOURCE_DIR)/$(object).c;)

# Generated variables to assist in make target declaration
C_HEADER_FILES := $(wildcard $(C_INCLUDE_DIR)/*.h)
C_TEST_FILES := $(wildcard $(C_TEST_DIR)/*.cpp)
GCC_OUTPUT_FILES := $(foreach object, $(C_OBJECT_NAMES), $(GCC_BUILD_DIR)/$(object).o)
CLANG_OUTPUT_FILES := $(foreach object, $(C_OBJECT_NAMES), $(CLANG_BUILD_DIR)/$(object).o)

# The main targets are grouped togheter into 6 build phases (Hello Maven)
# 1) Build directory structure creation
$(BUILD_DIR):
	mkdir $@;

$(GCC_BUILD_DIR): | $(BUILD_DIR)
	mkdir $@;

$(CLANG_BUILD_DIR): | $(BUILD_DIR)
	mkdir $@;

$(GTEST_BUILD_DIR): | $(BUILD_DIR)
	mkdir $@;

# 2) Source Compilation
# C only since the C++ code is all templates (headers)
$(GCC_OUTPUT_FILES): $(C_SOURCE_FILES) $(C_HEADER_FILES) | $(GCC_BUILD_DIR)
	$(GCC_COMPILE)

$(CLANG_OUTPUT_FILES): $(C_SOURCE_FILES) $(C_HEADER_FILES) | $(CLANG_BUILD_DIR)
	$(CLANG_COMPILE)

# 3) Library Generation
# C only since the C++ code is all templates (headers)
$(GCC_LIB): $(GCC_OUTPUT_FILES)
	ar -crs $@ $(GCC_BUILD_DIR)/*.o;

$(CLANG_LIB): $(CLANG_OUTPUT_FILES)
	ar -crs $@ $(CLANG_BUILD_DIR)/*.o;

# 4) Test Compilation
# 
# I opted to not use -l and -L in the test compilation targets in favor
# of directly referring to the gtest and kwazcoll libs.
# 
# Right now this makes sense because the full lib paths are
# convieniently defined as variables in the prequisite lists.
#
# Google test itself is only compiled once via g++.
$(GTEST_LIB): | $(GTEST_BUILD_DIR)
	$(GTEST_COMPILE) -pthread -I $(GTEST_INCLUDE_DIR) -I $(GTEST_DIR) -o $(GTEST_BUILD_DIR)/gtest-all.o -c $(GTEST_DIR)/src/gtest-all.cc;
	$(GTEST_COMPILE) -pthread -I $(GTEST_INCLUDE_DIR) -I $(GTEST_DIR) -o $(GTEST_BUILD_DIR)/gtest_main.o -c $(GTEST_DIR)/src/gtest_main.cc;
	ar -crs $@ $(GTEST_BUILD_DIR)/*.o;

$(GCC_TEST_EXEC): $(GCC_LIB) $(GTEST_LIB) $(C_TEST_FILES)
	$(GPP_TEST_COMPILE) -I $(GTEST_INCLUDE_DIR) -I $(C_INCLUDE_DIR) -o $@ -pthread $(C_TEST_SUITE) $< $(word 2, $^);

$(CLANG_TEST_EXEC): $(CLANG_LIB) $(GTEST_LIB) $(C_TEST_FILES)
	$(CLANGPP_TEST_COMPILE) -I $(GTEST_INCLUDE_DIR) -I $(C_INCLUDE_DIR) -o $@ -pthread $(C_TEST_SUITE) $< $(word 2, $^);

# 5) Test Execution
.PHONY: gcctestrun
gcctestrun: $(GCC_TEST_EXEC)
	$<;

.PHONY: clangtestrun
clangtestrun: $(CLANG_TEST_EXEC)
	$<;

# 6) Aggregation Tasks (These are what you want to run)
.PHONY: gcc
gcc: gcctestrun

.PHONY: clang
clang: clangtestrun

.PHONY: all
all: gcc clang

# Valgrind Tasks
.PHONY: valgrind
valgrind: all
	$(VALGRIND) $(GCC_TEST_EXEC);
	$(VALGRIND) $(CLANG_TEST_EXEC);

# Obligatory clean task
.PHONY: clean
clean:
	if [ -d $(BUILD_DIR) ]; then rm -r $(BUILD_DIR); fi;
