# The Art of C++
# Copyright (c) 2016-2020 Daniel Frey
# Please see LICENSE for license or visit https://github.com/taocpp/taopq

.SUFFIXES:
.SECONDARY:

ifeq ($(OS),Windows_NT)
UNAME_S := $(OS)
ifeq ($(shell gcc -dumpmachine),mingw32)
MINGW_CXXFLAGS = -U__STRICT_ANSI__
endif
else
UNAME_S := $(shell uname -s)
endif

# For Darwin (Mac OS X / macOS) we assume that the default compiler
# clang++ is used; when $(CXX) is some version of g++, then
# $(CXXSTD) has to be set to -std=c++17 (or newer) so
# that -stdlib=libc++ is not automatically added.

ifeq ($(CXXSTD),)
CXXSTD := -std=c++17
ifeq ($(UNAME_S),Darwin)
CXXSTD += -stdlib=libc++
endif
endif

# Ensure strict standard compliance and no warnings, can be
# changed if desired.

BUILDDIR ?= build

INCFLAGS ?= -Iinclude $(patsubst %,-I%,$(shell pg_config --includedir))
CPPFLAGS ?= -pedantic
CXXFLAGS ?= -Wall -Wextra -Wshadow -Werror -O3 $(MINGW_CXXFLAGS)
LDFLAGS ?= -rdynamic $(patsubst %,-L%,$(shell pg_config --libdir))
LIBS ?= -lpq

CLANG_TIDY ?= clang-tidy

HEADERS := $(shell find include -name '*.hpp')
SOURCES := $(shell find src -name '*.cpp')
DEPENDS := $(SOURCES:%.cpp=$(BUILDDIR)/%.d)
BINARIES := $(SOURCES:%.cpp=$(BUILDDIR)/%)

CLANG_TIDY_HEADERS := $(filter-out include/tao/pq/internal/endian_win.hpp,$(HEADERS))

UNIT_TESTS := $(filter $(BUILDDIR)/src/test/%,$(BINARIES))

LIBSOURCES := $(filter src/lib/%,$(SOURCES))
LIBNAME := taopq

.PHONY: all
all: check

.PHONY: compile
compile: $(UNIT_TESTS)

.PHONY: check
check: $(UNIT_TESTS)
	@set -e; for T in $(UNIT_TESTS); do echo $$T; $$T; done

$(BUILDDIR)/%.clang-tidy: % .clang-tidy
	$(CLANG_TIDY) -quiet $< -- $(CXXSTD) $(INCFLAGS) $(CPPFLAGS) $(CXXFLAGS) 2>/dev/null
	@mkdir -p $(@D)
	@touch $@

.PHONY: clang-tidy
clang-tidy: $(CLANG_TIDY_HEADERS:%=$(BUILDDIR)/%.clang-tidy) $(SOURCES:%=$(BUILDDIR)/%.clang-tidy)
	@echo "All $(words $(CLANG_TIDY_HEADERS) $(SOURCES)) clang-tidy tests passed."

.PHONY: clean
clean:
	@rm -rf $(BUILDDIR)/*
	@find . -name '*~' -delete

$(BUILDDIR)/%.d: %.cpp Makefile
	@mkdir -p $(@D)
	$(CXX) $(CXXSTD) $(INCFLAGS) $(CPPFLAGS) -MM -MQ $@ $< -o $@

$(BUILDDIR)/%.o: %.cpp $(BUILDDIR)/%.d
	$(CXX) $(CXXSTD) $(INCFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/lib/lib$(LIBNAME).a: $(LIBSOURCES:%.cpp=$(BUILDDIR)/%.o)
	@mkdir -p $(@D)
	$(AR) -rcs $@ $^

.PHONY: lib
lib: $(BUILDDIR)/lib/lib$(LIBNAME).a

$(BUILDDIR)/%: $(BUILDDIR)/%.o $(BUILDDIR)/lib/lib$(LIBNAME).a
	@mkdir -p $(@D)
	$(CXX) $(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o $@

ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(DEPENDS)
endif
