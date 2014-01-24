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

# Test executables
TEST_EXEC_NAME := tests.run
GCC_TEST_EXEC := $(GCC_BUILD_DIR)/$(TEST_EXEC_NAME)
GPP_TEST_EXEC := $(GPP_BUILD_DIR)/$(TEST_EXEC_NAME)
CLANG_TEST_EXEC := $(CLANG_BUILD_DIR)/$(TEST_EXEC_NAME)
CLANGPP_TEST_EXEC := $(CLANGPP_BUILD_DIR)/$(TEST_EXEC_NAME)

# KwazCollections C library info
C_LIB_FILENAME := libckwazcoll.a
GCC_LIB := $(GCC_BUILD_DIR)/$(C_LIB_FILENAME)
CLANG_LIB := $(CLANG_BUILD_DIR)/$(C_LIB_FILENAME)

# KwazCOllections C++ library info
CPP_LIB_FILENAME := libcppkwazcoll.a
GPP_LIB := $(GPP_BUILD_DIR)/$(CPP_LIB_FILENAME)
CLANGPP_LIB := $(CLANGPP_BUILD_DIR)/$(CPP_LIB_FILENAME)

# Main directories
# TODO: Make separate makefiles for each lang?
C_DIR := ./c
CPP_DIR := ./cpp

# Source directories
C_SOURCE_DIR := $(C_DIR)/src
C_INCLUDE_DIR := $(C_DIR)/include
CPP_SOURCE_DIR := $(CPP_DIR)/src
CPP_INCLUDE_DIR := $(CPP_DIR)/include

# Test source directories and suites
C_TEST_DIR := $(C_DIR)/test
C_TEST_SUITE := $(C_TEST_DIR)/TestSuite.cpp
CPP_TEST_DIR := $(CPP_DIR)/test
CPP_TEST_SUITE := $(CPP_TEST_DIR)/TestSuite.cpp

# Google Test stuff
GTEST_BUILD_DIR := $(BUILD_DIR)/gtest
GTEST_DIR := ./lib/gtest-1.7.0
GTEST_INCLUDE_DIR := $(GTEST_DIR)/include
GTEST_LIB := $(GTEST_BUILD_DIR)/libgtest.a

# Compilation commands
CLANG := clang -Wall -Wextra -Werror -pedantic-errors -ansi -I $(C_INCLUDE_DIR) -c
CLANGPP := clang++ -Wall -Wextra -Werror -pedantic-errors -ansi -I $(CPP_INCLUDE_DIR) -c
GCC := gcc -Wall -Wextra -Werror -pedantic-errors -ansi -I $(C_INCLUDE_DIR) -c
GPP := g++ -Wall -Wextra -Werror -pedantic-errors -ansi -I $(CPP_INCLUDE_DIR) -c

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
CPP_HEADER_FILES := $(wildcard $(CPP_INCLUDE_DIR)/*.hpp)
C_TEST_FILES := $(wildcard $(C_TEST_DIR)/*.cpp)
CPP_TEST_FILES := $(wildcard $(CPP_TEST_DIR)/*.cpp)
GCC_OUTPUT_FILES := $(foreach object, $(C_OBJECT_NAMES), $(GCC_BUILD_DIR)/$(object).o)
CLANG_OUTPUT_FILES := $(foreach object, $(C_OBJECT_NAMES), $(CLANG_BUILD_DIR)/$(object).o)
GPP_OUTPUT_FILES := $(foreach object, $(CPP_OBJECT_NAMES), $(GPP_BUILD_DIR)/$(object).o)
CLANGPP_OUTPUT_FILES := $(foreach object, $(CPP_OBJECT_NAMES), $(CLANGPP_BUILD_DIR)/$(object).o)

# The main targets are grouped togheter into 6 build phases (Hello Maven)
# 1) Build directory structure creation
$(BUILD_DIR):
	mkdir $@;

$(GCC_BUILD_DIR): | $(BUILD_DIR)
	mkdir $@;

$(GPP_BUILD_DIR): | $(BUILD_DIR)
	mkdir $@;

$(CLANG_BUILD_DIR): | $(BUILD_DIR)
	mkdir $@;

$(CLANGPP_BUILD_DIR): | $(BUILD_DIR)
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


$(GPP_TEST_EXEC): $(GTEST_LIB) $(CPP_TEST_FILES) $(CPP_HEADER_FILES) | $(GPP_BUILD_DIR)
	$(GPP_TEST_COMPILE) -I $(GTEST_INCLUDE_DIR) -I $(CPP_INCLUDE_DIR) -o $@ -pthread $(CPP_TEST_SUITE) $<;


$(CLANG_TEST_EXEC): $(CLANG_LIB) $(GTEST_LIB) $(C_TEST_FILES)
	$(CLANGPP_TEST_COMPILE) -I $(GTEST_INCLUDE_DIR) -I $(C_INCLUDE_DIR) -o $@ -pthread $(C_TEST_SUITE) $< $(word 2, $^);


$(CLANGPP_TEST_EXEC): $(GTEST_LIB) $(CPP_TEST_FILES) $(CPP_HEADER_FILES) | $(CLANGPP_BUILD_DIR)
	$(CLANGPP_TEST_COMPILE) -I $(GTEST_INCLUDE_DIR) -I $(CPP_INCLUDE_DIR) -o $@ -pthread $(CPP_TEST_SUITE) $<;

# 5) Test Execution
.PHONY: gcctestrun
gcctestrun: $(GCC_TEST_EXEC)
	$<;

.PHONY: gpptestrun
gpptestrun: $(GPP_TEST_EXEC)
	$<;

.PHONY: clangtestrun
clangtestrun: $(CLANG_TEST_EXEC)
	$<;

.PHONY: clangpptestrun
clangpptestrun: $(CLANGPP_TEST_EXEC)
	$<;

# 6) Aggregation Tasks (These are what you want to run)
.PHONY: c
c: gcctestrun clangtestrun

.PHONY: cpp
cpp: gpptestrun clangpptestrun

.PHONY: gnu
gnu: gcctestrun gpptestrun

.PHONY: clang
clang: clangtestrun clangpptestrun

.PHONY: all
all: c cpp

# Valgrind Tasks
.PHONY: cvalgrind
cvalgrind: c
	$(VALGRIND) $(GCC_TEST_EXEC);
	$(VALGRIND) $(CLANG_TEST_EXEC);

.PHONY: cppvalgrind
cppvalgrind: cpp
	$(VALGRIND) $(GPP_TEST_EXEC);
	$(VALGRIND) $(CLANGPP_TEST_EXEC);

# Run all possible make tasks
.PHONY: complete
complete: cvalgrind cppvalgrind

# Obligatory clean task
.PHONY: clean
clean:
	if [ -d $(BUILD_DIR) ]; then rm -r $(BUILD_DIR); fi;
