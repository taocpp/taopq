# The Art of C++
# Copyright (c) 2016-2018 Daniel Frey
# Please see LICENSE for license or visit https://github.com/taocpp/postgres

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
# $(CXXSTD) has to be set to -std=c++11 (or newer) so
# that -stdlib=libc++ is not automatically added.

ifeq ($(CXXSTD),)
CXXSTD := -std=c++11
ifeq ($(UNAME_S),Darwin)
CXXSTD += -stdlib=libc++
endif
endif

# Ensure strict standard compliance and no warnings, can be
# changed if desired.

INCFLAGS ?= -Iinclude $(patsubst %,-I%,$(shell pg_config --includedir))
CPPFLAGS ?= -pedantic
CXXFLAGS ?= -Wall -Wextra -Wshadow -Werror -O3 $(MINGW_CXXFLAGS)
LDFLAGS ?= -rdynamic $(patsubst %,-L%,$(shell pg_config --libdir))
LIBS ?= -lpq

BUILDDIR ?= build

CLANG_TIDY ?= clang-tidy

HEADERS := $(filter-out include/tao/optional/akrzemi1/%,$(shell find include -name '*.hpp')) $(filter-out src/test/macros.hpp,$(shell find src -name '*.hpp'))
SOURCES := $(shell find src -name '*.cpp')
DEPENDS := $(SOURCES:%.cpp=$(BUILDDIR)/%.d)
BINARIES := $(SOURCES:%.cpp=$(BUILDDIR)/%)

UNIT_TESTS := $(filter $(BUILDDIR)/src/test/%,$(BINARIES))

LIBSOURCES := $(filter src/lib/%,$(SOURCES))
LIBNAME := taopq

.PHONY: all
all: check

.PHONY: compile
compile: $(UNIT_TESTS)

.PHONY: check
check: compile
	@set -e; for T in $(UNIT_TESTS); do echo $$T; $$T; done

.PHONY: clean
clean:
	@rm -rf $(BUILDDIR)
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

build/%.clang-tidy: %
	$(CLANG_TIDY) -extra-arg "-Iinclude" -extra-arg "-std=c++11" -checks=*,-google-runtime-references,-google-runtime-int,-google-readability-todo,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-modernize-raw-string-literal,-misc-sizeof-expression -warnings-as-errors=* $< 2>/dev/null
	@mkdir -p $(@D)
	@touch $@

.PHONY: clang-tidy
clang-tidy: $(HEADERS:%=build/%.clang-tidy) $(SOURCES:%=build/%.clang-tidy)
	@echo "All $(words $(HEADERS) $(SOURCES)) clang-tidy tests passed."

ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(DEPENDS)
endif
