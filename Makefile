# The Art of C++
# Copyright (c) 2016 Daniel Frey

.SUFFIXES:
.SECONDARY:

ifeq ($(OS),Windows_NT)
UNAME_S := $(OS)
else
UNAME_S := $(shell uname -s)
endif

INCFLAGS ?= -Iinclude $(patsubst %,-I%,$(shell pg_config --includedir))
CPPFLAGS ?= -pedantic
CXXSTD ?= c++11
CXXFLAGS ?= -O3 -Wall -Wextra -Werror
LDFLAGS ?= -rdynamic $(patsubst %,-L%,$(shell pg_config --libdir))
LIBS ?= -lpq

BUILDDIR ?= build

SOURCES := $(shell find src/ -name '*.cpp')
DEPENDS := $(SOURCES:%.cpp=$(BUILDDIR)/%.d)

LIBSOURCES := $(filter src/lib/%,$(SOURCES))
LIBNAME := taopq

BINARIES := $(SOURCES:%.cpp=$(BUILDDIR)/%)
UNIT_TESTS := $(filter $(BUILDDIR)/src/test/%,$(BINARIES))

.PHONY: all
all: test

.PHONY: clean
clean:
	@rm -rf $(BUILDDIR)
	@find . -name '*~' -delete

$(BUILDDIR)/%.d: %.cpp Makefile
	@mkdir -p $(@D)
	$(CXX) -std=$(CXXSTD) $(CPPFLAGS) $(INCFLAGS) -MM -MQ $@ $< -o $@

$(BUILDDIR)/%.o: %.cpp $(BUILDDIR)/%.d
	$(CXX) -std=$(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $(INCFLAGS) -c $< -o $@

$(BUILDDIR)/lib/lib$(LIBNAME).a: $(LIBSOURCES:%.cpp=$(BUILDDIR)/%.o)
	@mkdir -p $(@D)
	$(AR) -rcs $@ $^

.PHONY: lib
lib: $(BUILDDIR)/lib/lib$(LIBNAME).a

$(BUILDDIR)/%: $(BUILDDIR)/%.o $(BUILDDIR)/lib/lib$(LIBNAME).a
	@mkdir -p $(@D)
	$(CXX) -std=$(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o $@

$(BUILDDIR)/%.success: $(BUILDDIR)/%
	$< >$<.log 2>&1
	touch $@

.PHONY: test
test: $(UNIT_TESTS:%=%.success)

ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(DEPENDS)
endif
