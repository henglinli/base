# -*-coding:utf-8-unix;-*-
# A sample Makefile for building Google Test and using it in user
# tests.  Please tweak it to suit your environment and project.  You
# may want to move it to your project's root directory.
#
# SYNOPSIS:
#
#   make [all]  - makes everything.
#   make TARGET - makes the given target.
#   make clean  - removes all files generated by make.

# Please tweak the following variable definitions as needed by your
# project, except GTEST_HEADERS, which you can use in your own targets
# but shouldn't modify.

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = third_party/googletest/googletest

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include -DGTEST_USE_OWN_TR1_UPLE=1
#
ASAN += -fsanitize=address -fno-omit-frame-pointer
RAGEL = ragel
CFLAGS += -Wall -Wextra -march=native $(ASAN)
CXXFLAGS += $(CFLAGS) -fno-exceptions -fno-rtti
LDFLAGS += -pthread
CPPFLAGS += -Iinclude
LOADLIBES += gtest_main.a
LDLIBS +=
#
TESTS_SRC = $(wildcard test/*_unittest.cpp)
#
# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = $(patsubst %.cpp, %.exe, $(TESTS_SRC))

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(wildcard $(GTEST_DIR)/include/gtest/*.h $(GTEST_DIR)/include/gtest/internal/*.h)

# House-keeping build targets.

all : gtest_main.a $(TESTS)
	./test/mpsc_unittest.exe
	./test/mpmc_unittest.exe
	./test/thread_unittest.exe
#
clean :
	$(RM) $(TESTS) gtest.a gtest_main.a *.o
#
# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(wildcard $(GTEST_DIR)/src/*.cc) $(wildcard $(GTEST_DIR)/src/*.h) $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
	    $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
	    $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# ragel rules
%.cc : %.rl
	$(RAGEL) $< -C -o $@

# test rules
%.exe : %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@
