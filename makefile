# A makefile for compiling KwazCollections under both the
# clang and/or gnu compilers
#
# The only special part of this file is the generated compilation commands.
#
# To get around c compiler restrictions of disallowing the use of -o with
# multiple input files, I use make functions to generate a list of compile
# commands into a set of variables. 
# These variables are then referenced in each compilation task.
#
# -David Kwasny

# Compiler (can be changed to gcc/g++)
C := clang
CPP := clang++

# Build directories
BUILD_DIR := build

# Test executable
TEST_EXEC := $(BUILD_DIR)/tests.run

# KwazCollections C library info
LIB_FILENAME := libckwazcoll.a
LIB := $(BUILD_DIR)/$(LIB_FILENAME)

# Source directories
SOURCE_DIR := src
INCLUDE_DIR := include

# Test source directories and suites
TEST_DIR := test
TEST_SUITE := $(TEST_DIR)/TestSuite.cpp

# Google Test stuff
GTEST_BUILD_DIR := $(BUILD_DIR)/gtest
GTEST_DIR := ./lib/gtest-1.7.0
GTEST_INCLUDE_DIR := $(GTEST_DIR)/include
GTEST_LIB := $(GTEST_BUILD_DIR)/libgtest.a

# Compilation commands
# I do not want to have to worry about my test code and google test
# passing the goofy ansi/pedantic crap I set for the main source.
COMPILE := $(C) -Wall -Wextra -Werror -pedantic-errors -ansi -I $(INCLUDE_DIR) -c
TEST_COMPILE := $(CPP) -Wall -Wextra -Werror
GTEST_COMPILE := $(CPP)

# Generated variables to assist in compilation
SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECT_NAMES := $(basename $(notdir $(SOURCE_FILES)))

# Generated variables to assist in make target declaration
HEADER_FILES := $(wildcard $(INCLUDE_DIR)/*.h)
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)
OUTPUT_FILES := $(foreach object, $(OBJECT_NAMES), $(BUILD_DIR)/$(object).o)

# Build Phases
# 1) Build directory structure creation
$(BUILD_DIR):
	mkdir $@;

$(GTEST_BUILD_DIR): | $(BUILD_DIR)
	mkdir $@;

# 2) Source Compilation
$(OUTPUT_FILES): $(SOURCE_FILES) $(HEADER_FILES) | $(BUILD_DIR)
	$(foreach object, $(OBJECT_NAMES), $(COMPILE) -o $(BUILD_DIR)/$(object).o $(SOURCE_DIR)/$(object).c;)

# 3) Library Generation
$(LIB): $(OUTPUT_FILES)
	ar -crs $@ $(BUILD_DIR)/*.o;

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

$(TEST_EXEC): $(LIB) $(GTEST_LIB) $(TEST_FILES)
	$(TEST_COMPILE) -I $(GTEST_INCLUDE_DIR) -I $(INCLUDE_DIR) -o $@ -pthread $(TEST_SUITE) $< $(word 2, $^);

# 5) Test Execution
.DEFAULT_GOAL := test
.PHONY: test
test: $(TEST_EXEC)
	$<;

# Run unit tests through valgrind
.PHONY: valgrind
valgrind: test
	valgrind --tool=memcheck --leak-check=full --error-exitcode=1 $(TEST_EXEC);

.PHONY: all
all: valgrind

.PHONY: clean
clean:
	if [ -d $(BUILD_DIR) ]; then rm -r $(BUILD_DIR); fi;
