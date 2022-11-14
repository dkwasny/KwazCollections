# Directories
SOURCE_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib
BUILD_DIR = build

# Test executable
TEST_EXEC = $(BUILD_DIR)/tests.run

# KwazCollections C library info
LIB_FILENAME = libckwazcoll.a
LIB = $(BUILD_DIR)/$(LIB_FILENAME)

# Test source directories and suites
TEST_DIR = test
TEST_SUITE = $(TEST_DIR)/TestSuite.cpp

# Google Test stuff
GTEST_DIR = $(LIB_DIR)/gtest
GTEST_ARCHIVE = $(GTEST_DIR)/gtest.tar.gz
GTEST_ARCHIVE_URL = https://github.com/google/googletest/archive/release-1.11.0.tar.gz
GTEST_LIB = $(BUILD_DIR)/libgtest_main.a
GTEST_MAIN_LIB = $(BUILD_DIR)/gtest_main.a
GTEST_INCLUDE_DIR = $(GTEST_DIR)/googletest/include

# Compilation commands
# I do not want to have to worry about my test code
# passing the goofy ansi/pedantic crap I set for the main source.
CFLAGS += -Wall -Wextra -Werror -pedantic-errors -ansi
COMPILE = $(CC) $(CFLAGS) $(CPPFLAGS) -I $(INCLUDE_DIR) -c
CXX_FLAGS += -Wall -Wextra -Werror
TEST_COMPILE = $(CXX) $(CXXFLAGS) $(CPPFLAGS)

# Generated variables to assist in rule creation
SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.c)
HEADER_FILES = $(wildcard $(INCLUDE_DIR)/*.h)
TEST_FILES = $(wildcard $(TEST_DIR)/*.cpp)
OUTPUT_FILES = $(foreach object, $(SOURCE_FILES), $(BUILD_DIR)/$(patsubst %.c,%.o,$(notdir $(object))))

# Build Phases
# 1) Build directory structure creation
$(BUILD_DIR):
	mkdir $@;

# 2) Source Compilation
$(OUTPUT_FILES): $(SOURCE_FILES) $(HEADER_FILES) | $(BUILD_DIR)
	$(COMPILE) -o $@ $(SOURCE_DIR)/$(patsubst %.o,%.c,$(notdir $@));

# 3) Library Generation
$(LIB): $(OUTPUT_FILES)
	$(AR) $(ARFLAGS) $@ $(BUILD_DIR)/*.o;

# 4) Test Compilation
$(GTEST_DIR):
	mkdir $@;
	wget -O $(GTEST_ARCHIVE) $(GTEST_ARCHIVE_URL);
	tar -x -C $@ -f $(GTEST_ARCHIVE) --strip-components 1;
	(cd $@ && cmake . && make);

$(GTEST_MAIN_LIB): $(GTEST_DIR)
	cp $(GTEST_DIR)/lib/libgtest_main.a $@;

$(GTEST_LIB): $(GTEST_DIR)
	cp $(GTEST_DIR)/lib/libgtest.a $@;

$(TEST_EXEC): $(LIB) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(TEST_FILES)
	$(TEST_COMPILE) -I $(GTEST_INCLUDE_DIR) -I $(INCLUDE_DIR) -o $@ -pthread $(TEST_SUITE) $< $(word 2, $^) $(word 3, $^);

# 5) Test Execution
.DEFAULT_GOAL = test
.PHONY: test
test: $(TEST_EXEC)
	$<;

# Run unit tests through valgrind
.PHONY: valgrind
valgrind: $(TEST_EXEC)
	valgrind --tool=memcheck --leak-check=full --error-exitcode=1 $(TEST_EXEC);

.PHONY: all
all: valgrind

.PHONY: clean
clean:
	if [ -d $(BUILD_DIR) ]; then rm -r $(BUILD_DIR); fi;
