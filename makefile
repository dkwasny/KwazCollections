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

# Build directories
BUILD_DIR = build

# Test executable
TEST_EXEC = $(BUILD_DIR)/tests.run

# KwazCollections C library info
LIB_FILENAME = libckwazcoll.a
LIB = $(BUILD_DIR)/$(LIB_FILENAME)

# Source directories
SOURCE_DIR = src
INCLUDE_DIR = include

# Test source directories and suites
TEST_DIR = test
TEST_SUITE = $(TEST_DIR)/TestSuite.cpp

# Google Test stuff
GTEST_DIR = lib/gtest-1.7.0
GTEST_INCLUDE_DIR = $(GTEST_DIR)/include
GTEST_LIB = $(BUILD_DIR)/gtest_main.a

# Compilation commands
# I do not want to have to worry about my test code
# passing the goofy ansi/pedantic crap I set for the main source.
CFLAGS += -Wall -Wextra -Werror -pedantic-errors -ansi
COMPILE = $(CC) $(CFLAGS) $(CPPFLAGS) -I $(INCLUDE_DIR) -c
CXX_FLAGS += -Wall -Wextra -Werror
TEST_COMPILE = $(CXX) $(CXXFLAGS) $(CPPFLAGS)

# Generated variables to assist in compilation
SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECT_NAMES = $(basename $(notdir $(SOURCE_FILES)))

# Generated variables to assist in make target declaration
HEADER_FILES = $(wildcard $(INCLUDE_DIR)/*.h)
TEST_FILES = $(wildcard $(TEST_DIR)/*.cpp)
OUTPUT_FILES = $(foreach object, $(OBJECT_NAMES), $(BUILD_DIR)/$(object).o)

# Build Phases
# 1) Build directory structure creation
$(BUILD_DIR):
	mkdir $@;

# 2) Source Compilation
$(OUTPUT_FILES): $(SOURCE_FILES) $(HEADER_FILES) | $(BUILD_DIR)
	$(foreach object, $(OBJECT_NAMES), $(COMPILE) -o $(BUILD_DIR)/$(object).o $(SOURCE_DIR)/$(object).c;)

# 3) Library Generation
$(LIB): $(OUTPUT_FILES)
	$(AR) $(ARFLAGS) $@ $(BUILD_DIR)/*.o;

# 4) Test Compilation
$(GTEST_LIB):
	(cd $(GTEST_DIR)/make && make);
	cp $(GTEST_DIR)/make/gtest_main.a $(GTEST_LIB);

$(TEST_EXEC): $(LIB) $(GTEST_LIB) $(TEST_FILES)
	$(TEST_COMPILE) -I $(GTEST_INCLUDE_DIR) -I $(INCLUDE_DIR) -o $@ -pthread $(TEST_SUITE) $< $(word 2, $^);

# 5) Test Execution
.DEFAULT_GOAL = test
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
