# At the moment, this is a minimum viable Makefile. It makes no pretence of generality or portability, and simply
# provides enough machinery to build Spidermonkey on my Ubuntu box.

# It seems to me that we likely want a way to say "Hey, don't build the Spidermonkey libs, I have them lying around
# already at ... ". (Note to self: it seems SpiderMonkey creates a js.pc file, so we can use pkg-config's existence
# test to conditionally build if not present. How will that interact with using vpath to search for the .so given
# that the library will be in the user's default? install location in that case? In that case, would we want to move
# to simply require the user to have a SpiderMonkey library whose version is discoverable to pkg-config? If we go down
# that route do we even need pkg-config? Doesn't autoconf allow checking for libraries and their versions?)
# 
# Of course, at time of writing, I also need to investigate what node actually expects in terms of v8 libraries, (i.e
# shared or static?) which in turn leads to questions regarding the scope of my v8monkey: should it simply satisfy
# node's V8 embedding requirements or do we want it to be fully general for any V8 embedder?


# XXX Debug doesn't seem to be working. Certainly the compiled libraries don't have, for example, the JS_SetGCZeal symbol
#     which should be present in debug builds. (v8monkey bits get compiled correctly, and js/src/configure does get called
#     with --enable-debug --disable-optimize, but we still seem to end up with a standard SpiderMonkey build)

.PHONY: all echoname clean clobber check


# Directory containing upstream code
mozillaroot = $(CURDIR)/deps/mozilla_upstream


# Command to invoke to discover Gecko/SpiderMonkey version
milestonecommand = $(mozillaroot)/config/milestone.pl


# Major version number
smmajorversion = $(shell $(milestonecommand) --topsrcdir=$(mozillaroot) --symbolversion)


# Full version number
smfullversion = $(shell $(milestonecommand) --topsrcdir=$(mozillaroot))


# Name of the SpiderMonkey library for linking purposes
smlinklib = mozjs-$(smmajorversion)


# Full name of the SpiderMonkey shared library
smlibrary = lib$(smlinklib).so


# Link name of the library we will build
v8lib = v8monkey-$(smfullversion)


# Link name of the internal testing version of the library which exports additional symbols useful for testing
v8testlib = v8monkey-test-$(smfullversion)


# Define the debug macro and rename the library in debug builds
ifdef DEBUG
v8lib = v8monkey-debug-$(smfullversion)
CXXFLAGS += -DDEBUG -g -DJS_DEBUG
endif


# TODO: Fix soname of library
v8monkeylibrary = lib$(v8lib).so
v8monkeytestlibrary = lib$(v8testlib).so


# Where shall we place our artifacts?
# XXX Is there an idiomatic name for this in make?
OUTDIR ?= dist


# Build in another directory in the case of debug builds
# XXX This overrides anything user-specified: how do I handle that case?
ifdef DEBUG
OUTDIR = dist/debug
endif


# Where should we build SpiderMonkey in the output dir?
DEPSBUILD ?= deps
depsout = $(OUTDIR)/$(DEPSBUILD)
depheaders = $(depsout)/dist/include
deplib = $(depsout)/dist/lib


# Enable all warnings, position-independent code and C++11 when compiling the individual object files
# Note the use of -i system to treat the JS headers as "system" headers, which turns off warning spew from those files
# XXX See if that is still required once we have broken the JSAPI dependence noted below
CXXFLAGS +=-Wall -Wextra -Wmissing-include-dirs -fPIC -isystem $(depheaders) -I include -std=c++11


# Look for shared libraries in dist
# XXX Will likely become redundant if we go down the "build your own damn SpiderMonkey lib" route noted at top)
vpath %.so $(OUTDIR) $(deplib)


# Our default goal is to build the v8monkey shared library and the infrastructure for running tests
all: $(OUTDIR)/$(v8monkeylibrary) $(OUTDIR)/test/run_v8monkey_tests $(OUTDIR)/test/run_v8monkey_internal_tests temp


# XXX Remove me!
temp: $(OUTDIR)/$(v8monkeylibrary) temp.cpp
	$(CXX) -o temp temp.cpp -std=c++0x -I $(OUTDIR)/deps/dist/include -Wl,-L$(CURDIR)/$(OUTDIR) -Wl,-rpath=$(CURDIR)/$(OUTDIR) -l$(v8lib)


# The files that compose libv8monkey
filestems = isolate init version platform
files = $(addprefix src/, $(filestems))
sources = $(addsuffix .cpp, $(files))
objects = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(files)))


# Individual header file dependencies below


# isolate.cpp and init.cpp additionally depend on init.h
src/isolate.cpp src/init.cpp: src/init.h


# isolate.cpp and init.cpp additionally depend on the JSAPI header
src/isolate.cpp src/init.cpp: $(depheaders)/jsapi.h


# version.cpp and test/test_version.cpp needs SMVERSION defined
$(OUTDIR)/src/version.o $(OUTDIR)/testlib/version.o $(OUTDIR)/test/test_version.o: CXXFLAGS += -DSMVERSION='"$(smfullversion)"'


# Several files depend on platform capabilities
src/init.cpp test/test_platform.cpp src/autolock.h: src/platform.h


# Several files depend on the autolock RAII class
src/init.cpp: src/autolock.h


# The individual object files depend on the existence of their output directory
$(objects): | $(OUTDIR)/src


# Create object file output directory if required
$(OUTDIR)/src:
	mkdir -p $(OUTDIR)/src


# Originally, I declared a vpath for %.o to be dist, however, it is my understanding that this messes up implicit
# rules: i.e it would take the stem as-for example-dist/isolate rather than isolate and look for a prereq of
# dist/isolate.cpp rather than using the vpath for cpp files above (or rather it would look for src/dist/isolate.cpp)
# TODO Find a make guru and check if that is correct
$(OUTDIR)/%.o: %.cpp include/v8.h
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<


# Our default target is the shared library
# XXX Move the pthread link, factor out platform-specific bits
$(OUTDIR)/$(v8monkeylibrary): $(objects) $(OUTDIR)/include/v8.h $(smlibrary)
	$(CXX) -shared -Wl,-soname,$(v8monkeylibrary) -Wl,-L$(deplib) -Wl,-rpath=$(CURDIR)/$(deplib) -o \
			$(OUTDIR)/$(v8monkeylibrary) $(objects) -l$(smlinklib) -lpthread


# XXX Need to break the dependence of the include header with JSAPI.h
$(OUTDIR)/include/v8.h: include/v8.h | $(OUTDIR)/include
	cp include/v8.h $(OUTDIR)/include


# XXX Need to break the dependence of the include header with JSAPI.h
include/v8.h: $(depsout)/dist/include/jsapi.h


# Ensure the include directory exists
$(OUTDIR)/include:
	mkdir -p $(OUTDIR)/include


# The files that compose the test harness
teststems = V8MonkeyTest test_version
testfiles = $(addprefix test/, $(teststems))
testsources = $(addsuffix .cpp, $(testfiles))
testobjects = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(testfiles)))


# The files that compose the "internal" test harness
internalteststems = V8MonkeyTest test_internal test_platform
internaltestfiles = $(addprefix test/, $(internalteststems))
internaltestsources = $(addsuffix .cpp, $(internaltestfiles))
internaltestobjects = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(internaltestfiles)))
$(internaltestobjects): CXXFLAGS += -I src


# V8MonkeyTest and the test harnesses depend on the V8MonkeyTest header
$(OUTDIR)/test/V8MonkeyTest.o $(OUTDIR)/test/run_v8monkey_tests $(OUTDIR)/test/run_v8monkey_internal_tests: test/V8MonkeyTest.h


# Ensure the directory is created for test files
$(testobjects) $(internaltestobjects): | $(OUTDIR)/test
$(OUTDIR)/test:
	mkdir -p $(OUTDIR)/test


# Build the test harness
$(OUTDIR)/test/run_v8monkey_tests $(OUTDIR)/test/run_v8monkey_internal_tests: CXXFLAGS += -I test
$(OUTDIR)/test/run_v8monkey_tests: test/run_v8monkey_tests.cpp $(testobjects) $(OUTDIR)/$(v8monkeylibrary)
	$(CXX) $(CXXFLAGS) -o $(OUTDIR)/test/run_v8monkey_tests test/run_v8monkey_tests.cpp $(testobjects)  -Wl,-L$(OUTDIR) -Wl,-rpath=$(CURDIR)/$(OUTDIR) -l$(v8lib)


# Extra files for the internal test version of the library
testlibstems = $(filestems) testAPI
testlibfiles = $(addprefix src/, $(testlibstems))
testlibsources = $(addsuffix .cpp, $(testlibfiles))
testlibobjects = $(addprefix $(OUTDIR)/testlib/, $(addsuffix .o, $(testlibstems)))


$(OUTDIR)/testlib/%.o: src/%.cpp include/v8.h | $(OUTDIR)/testlib
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<


# Ensure the testlib directory exists
$(OUTDIR)/testlib:
	mkdir -p $(OUTDIR)/testlib


# Add an additional define when compiling object files for the test lib
$(testlibobjects) $(internaltestobjects): CXXFLAGS += -DV8MONKEY_INTERNAL_TEST=1


# testAPI.cpp and internal tests need the testAPI header
testAPI.cpp $(internaltestsources): src/testAPI.h


# Build a version of the shared library for internal tests
$(OUTDIR)/test/$(v8monkeytestlibrary): $(testlibobjects) $(OUTDIR)/include/v8.h $(smlibrary)
	$(CXX) -shared -Wl,-soname,$(v8monkeytestlibrary) -Wl,-L$(deplib) -Wl,-rpath=$(CURDIR)/$(deplib) -o \
			$(OUTDIR)/test/$(v8monkeytestlibrary) $(testlibobjects) -l$(smlinklib)


# Build the internal test harness
# Note: we reuse the driver file run_v8monkey_tests.cpp. That isn't a copy/paste error
$(OUTDIR)/test/run_v8monkey_internal_tests: test/run_v8monkey_tests.cpp $(internaltestobjects) $(OUTDIR)/test/$(v8monkeytestlibrary)
	@echo $(internaltestobjects)
	$(CXX) $(CXXFLAGS) -o $(OUTDIR)/test/run_v8monkey_internal_tests test/run_v8monkey_tests.cpp $(internaltestobjects)  \
                       -Wl,-L$(OUTDIR)/test -Wl,-rpath=$(CURDIR)/$(OUTDIR)/test -l$(v8testlib)


# Code from here on down is concerned with building the SpiderMonkey shared library

# In the below, I was shelling out to the Mozilla make unneccesarily, as make apparently compares the real file's 
# timestamp when it encounters symlinks, and it turns out that the dist/include created by building SpiderMonkey
# contains symlinks. GNU make 3.81 provides a command-line flag to compare the symlink time, but we shouldn't require
# our users to remember to explicitly use such a flag. Thus, after an update, we touch the source jsapi.h header to fix
# the timestamp vs the Makefile

# If the library doesn't exist, we must explicitly build it
$(deplib)/libmozjs-33.so $(depheaders)/jsapi.h: $(depsout)/Makefile
	$(MAKE) -C $(depsout)
	touch $(mozillaroot)/js/src/jsapi.h


# To create the Makefile, we must have a config.status script
$(depsout)/Makefile: $(depsout)/config.status
	cd $(depsout) && $(mozillaroot)/js/src/configure


# Ensure appropriate options are passed for debug builds
configureopts =
ifdef DEBUG
configureopts := --enable-debug --disable-optimize
endif


# A config.status script is created by running configure
$(depsout)/config.status: $(mozillaroot)/js/src/configure | $(depsout)
	echo CONFIGURING WITH $(configureopts)
	cd $(depsout) && $(mozillaroot)/js/src/configure $(configureopts)


# To run configure we must first invoke autoconf
$(mozillaroot)/js/src/configure:
	cd $(mozillaroot)/js/src && autoconf


# Ensure deps output directory exists
$(depsout):
	mkdir -p $(depsout)


# Run the testsuite
check: $(OUTDIR)/test/run_v8monkey_tests
	@echo
	@echo Running API tests
	@$(OUTDIR)/test/run_v8monkey_tests
	@echo
	@echo Running internal tests
	@$(OUTDIR)/test/run_v8monkey_internal_tests


clean:
	rm -rf $(OUTDIR)/include
	rm -f $(OUTDIR)/*.o
	rm -f $(OUTDIR)/*.so


clobber:
	rm -rf $(OUTDIR)/*
	rm -f $(mozillaroot)/js/src/configure
