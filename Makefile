# At the moment, this is a minimum viable Makefile. It makes no pretence of generality or portability, and simply
# provides enough machinery to build Spidermonkey and V8Monkey on my Ubuntu box.

# At time of writing, we build SpiderMonkey ourselves. I think I'm soon going to move to a version that expects the
# required library to already be installed on the builder's system. (This raises questions regarding how to detect
# that the installed SpiderMonkey library is the correct version. Note to self: research pkg-config: it might help).

# That however might be too prescriptive. We might want to add the ability to allow the user to say, "hey, the
# SpiderMonkey shared library lives at...".
#
# In either of the above cases, I think ultimately we would still want a target that says "yeah, I don't have that
# library. Go ahead and pull in the SpiderMonkey source and build it for me, would ya?"
# 
# Of course, at time of writing, I also need to investigate what node actually expects in terms of v8 libraries, (i.e
# shared or static?) which in turn leads to questions regarding the scope of my v8monkey: should it simply satisfy
# node's V8 embedding requirements or do we want it to be fully general for any V8 embedder?


.DEFAULT_GOAL = all

.PHONY: all clean clobber check

# TODO Provide a target or flag for debug builds. We need to ensure this builds a debug SpiderMonkey library too

# Where does our copy of the mozilla-release codebase live?
mozillaroot = $(CURDIR)/deps/mozilla_upstream


# There's a perl script in the Mozilla source tree we can use to programatically discover the current version
milestonecommand = $(mozillaroot)/config/milestone.pl


# Lets put the above command to use. The name of the SpiderMonkey library has the major version number as its suffix
smmajorversion = $(shell $(milestonecommand) --topsrcdir=$(mozillaroot) --symbolversion)


# Our own V8Monkey library will have a suffix which denotes the full SpiderMonkey version we were built from
smfullversion = $(shell $(milestonecommand) --topsrcdir=$(mozillaroot))


# Compute the name of the SpiderMonkey library that we can pass as a -l flag to the linker
smlib = mozjs-$(smmajorversion)


# Compute the full name of the shared library file that will be built when building SpiderMonkey
smlibraryfile = lib$(smlib).so


# Likewise, compute the name of our library for linking purposes
v8lib = v8monkey-$(smfullversion)


# We will also build a special version of the library for testing our internals. This version of the library will
# export additional symbols to aid testing. Yes, yes, I know, one should simply test the interface, but there's enough
# gnarly bookkeeping involved that I need to seek additional comfort
v8testlib = v8monkey-test-$(smfullversion)


# And their corresponding full names
v8libraryfile = lib$(v8lib).so
v8testlibraryfile = lib$(v8testlib).so


# We also build a shared library for platform abstractions, which can be used both by V8Monkey and the test harnesses
platformlib = v8monkey-platform
platformlibfile = lib$(platformlib).so


# TODO: Do we need to specify a SONAME for our libraries?


# Where shall we place our artifacts?
# TODO: Is there an idiomatic name for this in make?
# TODO: Allow user to specify this. How do we ensure its an absolute filepath? Much of the code below assumes absolute
#       filepaths?
outdir = $(CURDIR)/dist


# Where should we build SpiderMonkey in the output dir?
# TODO: Allow user to specify this if we continue with manual builds, or we add a "pull in the deps and build 'em" target
# TODO: Will also need revisiting if we change
depsdir = $(outdir)/deps


# Where will the JSAPI header be written when we build SpiderMonkey?
smheadersdir = $(depsdir)/dist/include


# What directory will the SpiderMonkey library be in after building?
smlibdir = $(depsdir)/dist/lib


# Absolute filename of the SpiderMonkey shared library
smtarget = $(smlibdir)/$(smlibraryfile)


# Where will we write the V8 header?
v8monkeyheadersdir = $(outdir)/include


# Absolute filename of the V8Monkey library
v8monkeytarget = $(outdir)/$(v8libraryfile)


# Absolute filename of the platform library
v8platformtarget = $(outdir)/$(platformlibfile)


# Directory where the "internal" library object files will be built
v8monkeyinternaldir = $(outdir)/test/internalLib

# Absolute filename of the internal test variant of the V8Monkey library
v8monkeytesttarget = $(v8monkeyinternaldir)/$(v8testlibraryfile)


# We will generally need headers from 3 places:
#  - We will often need the JSAPI header
#  - We will often need the V8 header
#  - some files in src/ may depend on additional headers in src
# Note: we use -isystem to prevent warning spew from JSAPI
# TODO: Is there a idiomatic name for this in makefiles?
includedirs = $(v8monkeyheadersdir) $(CURDIR)/src/threads $(CURDIR)/src/engine $(CURDIR)/src/runtime $(CURDIR)/src
includeopt = $(addprefix -I, $(includedirs))
includeopt += -isystem $(smheadersdir)


# What warnings do we want to enable?
# TODO: Is there an idiomatic name for this in makefiles?
# TODO: Do we want to allow the user to specify additional warnings? i.e. should this be additive?
warnings = -Wall -Wextra -Wmissing-include-dirs


# Flags to pass to the C++ compiler:
#   - std=c++0x SpiderMonkey uses some C++11 features in the header
#   - fvisibility=hidden We want to minimize the number of symbols exported
#   - fPIC shared libraries need position-independent code
CXXFLAGS += -g $(warnings) $(includeopt) -fPIC -fvisibility=hidden -std=c++0x


# Define a command that will produce a link command for the given library name
linkcommand = -Wl,-L$(strip $(1)) -Wl,-rpath=$(strip $(1)) -l$(strip $(2))


# Because we build normal and "internal" versions of the object files, expressing dependencies could become
# a bit of a chore
variants = $(outdir)/$(strip $(1)).o $(v8monkeyinternaldir)/$(strip $(1)).o


# A useful general rule for building files
$(outdir)/%.o: %.cpp $(v8monkeyheadersdir)/v8.h
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<


#**********************************************************************************************************************
# Sources


# The V8Monkey library is composed of the following files
enginestems = $(addprefix src/engine/, init version)
engineobjects = $(addsuffix .o, $(enginestems))

runtimestems = $(addprefix src/runtime/, isolate)
runtimeobjects = $(addsuffix .o, $(runtimestems))

#threadstems = $(addprefix src/threads/, threadID)
#threadobjects = $(addsuffix .o, $(threadstems))

allstems = $(enginestems) $(runtimestems) $(threadstems)
allobjects = $(engineobjects) $(runtimeobjects) $(threadobjects)

v8sources = $(addsuffix .cpp, $(allstems))
v8objects = $(addprefix $(outdir)/, $(allobjects))

# Extra files for the internal test version of the library
testlibobjects = $(addprefix $(v8monkeyinternaldir)/, $(allobjects))

# The platform abstraction library contains the following:
platformstems = platform
platformfiles = $(addprefix platform/, $(platformstems))
platformsources = $(addsuffix .cpp, $(platformfiles))
platformobjects = $(addprefix $(outdir)/, $(addsuffix .o, $(platformfiles)))


# Names of the test harness executables: we need these for the all target
testharnesses = run_v8monkey_tests run_v8monkey_internal_tests
testsuites = $(addprefix $(outdir)/test/, $(testharnesses))


#**********************************************************************************************************************
# Output directories


# Here we list all the output directories that will be created in dist


# Where are object files placed when we build them in their various guises?
srctree = engine runtime threads
srcdirs = $(addprefix src/, $(srctree))
objecthierarchy = $(addprefix $(outdir)/, $(srcdirs))
internalobjecthierarchy = $(addprefix $(v8monkeyinternaldir)/, $(srcdirs))


# Directories needed for building tests
testdirs = internalLib internal api harness
testhierarchy = $(addprefix $(outdir)/test/, $(testdirs))


# What are the toplevel directories in the output directory?
topleveldirs = platform test src include deps
toplevelhierarchy = $(addprefix $(outdir)/, $(topleveldirs))


# All required directories
outputdirs = $(objecthierarchy) $(internalobjecthierarchy) $(testhierarchy) $(toplevelhierarchy) $(v8monkeyinternaldir)/src $(outdir)


# Ensure they exist when necessary
$(outputdirs):
	mkdir -p $@


# Directory dependencies
# TODO There is likely a smarter way to do this
$(internalobjecthierarchy): | $(v8monkeyinternaldir)/src
$(v8monkeyinternaldir)/src: | $(v8monkeyinternaldir)
$(testhierarchy): | $(outdir)/test
$(objecthierarchy): | $(outdir)/src
$(toplevelhierarchy): | $(outdir)


# This doesn't contain all directory dependencies. They are spread through the file
# TODO Again, there must be a better way. Paging Makefile gurus.
$(addprefix $(outdir)/, $(engineobjects)): | $(outdir)/src/engine
$(addprefix $(outdir)/, $(runtimeobjects)): | $(outdir)/src/runtime
$(addprefix $(outdir)/, $(threadobjects)): | $(outdir)/src/threads
$(addprefix $(v8monkeyinternaldir)/, $(engineobjects)): | $(v8monkeyinternaldir)/src/engine
$(addprefix $(v8monkeyinternaldir)/, $(runtimeobjects)): | $(v8monkeyinternaldir)/src/runtime
$(addprefix $(v8monkeyinternaldir)/, $(threadobjects)): | $(v8monkeyinternaldir)/src/threads


#**********************************************************************************************************************
# Platform abstraction library


# To build the shared library, we need to build the platform
# TODO pthread is POSIX specific: need to factor this out
$(v8platformtarget): $(platformobjects) $(v8monkeyheadersdir)/platform.h
	@echo
	@echo Building platform abstraction library...
	@echo
	$(CXX) -shared -o $@ $(platformobjects) -lpthread


# The platform objects depend on the header
$(platformobjects): $(v8monkeyheadersdir)/platform.h | $(outdir)/platform


# Ensure the header is in the right place
$(v8monkeyheadersdir)/platform.h: platform/platform.h | $(v8monkeyheadersdir)
	cp platform/platform.h $(v8monkeyheadersdir)


#**********************************************************************************************************************
# V8Monkey


# Our default goal is to build the v8monkey shared library, and the testsuites
all: $(v8monkeytarget) $(testsuites)


# The main task is building the shared library
$(v8monkeytarget): $(v8objects) $(v8monkeyheadersdir)/v8.h $(smtarget) $(v8platformtarget)
	@echo
	@echo Building V8Monkey...
	@echo
	$(CXX) -shared -o $@ $(v8objects) $(call linkcommand, $(smlibdir), $(smlib)) $(v8platformtarget)


# We need to copy the V8 header to dist
$(v8monkeyheadersdir)/v8.h: include/v8.h | $(v8monkeyheadersdir)
	cp include/v8.h $(v8monkeyheadersdir)


# *********************************************************************************************************************
# Individual file dependencies below


# Virtually all files will depend on the v8 header
$(v8objects) $(testlibobjects): $(v8monkeyheadersdir)/v8.h

# version.cpp and test/test_version.cpp need SMVERSION defined
$(call variants, src/engine/version) $(outdir)/test/api/test_version.o: CXXFLAGS += -DSMVERSION='"$(smfullversion)"'


# init depends on the jsapi header
$(call variants, src/engine/init): $(smheadersdir)/jsapi.h


# init depends on the RAII autolock class
$(call variants, src/engine/init) $(call variants, src/runtime/isolate): src/threads/autolock.h


# Several files depend on isolate.h
$(call variants, src/init) $(call variants, src/runtime/isolate): src/runtime/isolate.h


# Several files depend on platform capabilities
src/autolock.h $(call variants, src/engine/init) $(call variants, src/runtime/isolate): $(v8monkeyheadersdir)/platform.h


# Several files depend on the miscellaneous functions in the V8MonkeyCommon class
$(call variants, src/engine/init) $(call variants, src/runtime/isolate): src/v8monkey_common.h

# Several files compile differently (exposing different symbols) depending on whether they are being compiled for the
# internal test lib or not
visibility_changes = $(call variants, src/engine/init) $(call variants, src/runtime/isolate)


# We can now declare the visibility changers dependent on a test header file
visibility_changes: src/test.h


# *********************************************************************************************************************
# Testsuites

# The test harness is composed from the following
teststems = test_death test_isolate test_threadID test_version
testfiles = $(addprefix test/api/, $(teststems))
testsources = $(addsuffix .cpp, $(testfiles))
testobjects = $(addprefix $(outdir)/, $(addsuffix .o, $(testfiles)))


# TODO Our little trick with building the linked list of functions is neat, but is sensitive to compilation order. It will
#      segfault if the test files are specified before the V8MonkeyTest file


# Build the "standard" test harness
$(outdir)/test/run_v8monkey_tests: test/harness/run_v8monkey_tests.cpp $(testobjects) $(outdir)/test/harness/V8MonkeyTest.o $(v8monkeytarget) $(platformtarget)
	@echo
	@echo Building API testsuite...
	@echo
	$(CXX) $(CXXFLAGS) -g -o $@ test/harness/run_v8monkey_tests.cpp $(outdir)/test/harness/V8MonkeyTest.o $(testobjects) \
                        $(call linkcommand, $(outdir), $(v8lib))\
                        $(call linkcommand, $(outdir), $(platformlib))


# The "internals" test harness is composed from the following
internalteststems = test_death_internal test_fatalerror_internal test_isolate_internal test_platform test_threadID_internal
internaltestfiles = $(addprefix test/internal/, $(internalteststems))
internaltestsources = $(addsuffix .cpp, $(internaltestfiles))
internaltestobjects = $(addprefix $(outdir)/, $(addsuffix .o, $(internaltestfiles)))


# TODO The directory prereq here isn't quite right
# XXX Really need to fix this. It's annoying me. Prereq for code reorg
$(outdir)/test/internalLib/%.o: %.cpp $(v8monkeyheadersdir)/v8.h | $(outdir)/test/internalLib/src
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<


# Build a version of the shared library for internal tests
$(v8monkeytesttarget): $(testlibobjects) $(outdir)/include/v8.h $(smtarget)
	@echo
	@echo Building internal version of V8Monkey...
	@echo
	$(CXX) -shared -o $@ $(testlibobjects) $(call linkcommand, $(smlibdir), $(smlib)) $(v8platformtarget)


# Build the internal test harness
# Note: we reuse the driver file run_v8monkey_tests.cpp. That isn't a copy/paste error
$(outdir)/test/run_v8monkey_internal_tests: test/harness/run_v8monkey_tests.cpp $(internaltestobjects) test/harness/V8MonkeyTest.cpp $(v8monkeytesttarget) $(v8platformtarget)
	@echo
	@echo Building internal testsuite...
	@echo
	$(CXX) $(CXXFLAGS) -o $@ test/harness/run_v8monkey_tests.cpp $(outdir)/test/harness/V8MonkeyTest.o $(internaltestobjects) \
                        $(call linkcommand, $(outdir)/test/internalLib, $(v8testlib)) \
                        $(call linkcommand, $(outdir), $(platformlib))


# *********************************************************************************************************************
#  Specific test file dependencies below


# Modify compilation of internal library versions
$(testlibobjects) $(internaltestobjects): CXXFLAGS += -DV8MONKEY_INTERNAL_TEST=1


# Test objects need to locate the V8MonkeyTest test header
$(testobjects) $(internaltestobjects) $(testsuites) $(outdir)/test/harness/V8MonkeyTest.o: CXXFLAGS += -I$(CURDIR)/test/harness


# All test files depend on the V8MonkeyTest header
$(testobjects) $(internaltestobjects) $(testsuites): test/harness/V8MonkeyTest.h


# The testsuites depend on the additional machinery supplied by V8MonkeyTest
$(testsuites): $(outdir)/test/harness/V8MonkeyTest.o


# Virtually all test files use threads
$(testobjects) $(internaltestobjects): $(v8monkeyheadersdir)/platform.h


# Various files depend on isolate.h
$(outdir)/test/api/test_isolate.o $(outdir)/test/internal/test_fatalerror_internal.o $(outdir)/test/internal/test_isolate_internal.o: src/runtime/isolate.h


# Most internal test files require the TestUtils class
$(internaltestobjects): src/test.h


# Some test files depend on the V8MonkeyCommon class
$(outdir)/test/internal/test_death_internal.o $(outdir)/test/internal/test_fatalerror_internal.o: src/v8monkey_common.h


# The individual object files depend on the existence of their output directory
$(testobjects): | $(outdir)/test/api


# ... as do the internal object files
$(internaltestobjects): | $(outdir)/test/internal


# V8MonkeyTest also depends on its output directory
$(outdir)/test/harness/V8MonkeyTest.o: $(outdir)/test/harness


# *********************************************************************************************************************
# Code from here on down is concerned with building the SpiderMonkey shared library

# In the below, I was shelling out to the Mozilla make unneccesarily, as make apparently compares the real file's 
# timestamp when it encounters symlinks, and it turns out that the dist/include created by building SpiderMonkey
# contains symlinks. GNU make 3.81 provides a command-line flag to compare the symlink time, but we shouldn't require
# our users to remember to explicitly use such a flag. Thus, after an update, we touch the source jsapi.h header to fix
# the timestamp vs the Makefile

# If the library doesn't exist, we must explicitly build it
$(smtarget) $(smheadersdir)/jsapi.h: $(depsdir)/Makefile
	@echo
	@echo Building SpiderMonkey...
	@echo
	$(MAKE) -C $(depsdir)
	touch $(mozillaroot)/js/src/jsapi.h


# To create the Makefile, we must have a config.status script
$(depsdir)/Makefile: $(depsdir)/config.status
	cd $(depsdir) && $(mozillaroot)/js/src/configure


# A config.status script is created by running configure
$(depsdir)/config.status: $(mozillaroot)/js/src/configure | $(depsdir)
	cd $(depsdir) && $(mozillaroot)/js/src/configure


# To run configure we must first invoke autoconf
$(mozillaroot)/js/src/configure:
	cd $(mozillaroot)/js/src && autoconf


# Run the testsuite
# XXX If we can't find a way to capture the number of tests ran, remove the summary lines
check: $(testsuites)
	@echo
	@echo Running API tests
	@$(outdir)/test/run_v8monkey_tests
	@echo
	@echo Running internal tests
	@$(outdir)/test/run_v8monkey_internal_tests
	@echo
	@echo Summary:
	@echo API: Registered Tests: $(shell $(outdir)/test/run_v8monkey_tests -c)
	@echo Internal: Registered Tests: $(shell $(outdir)/test/run_v8monkey_internal_tests -c)


clean:
	rm -f $(v8monkeytarget)
	rm -f $(v8platformtarget)
	rm -rf $(v8monkeyheadersdir)
	rm -rf $(outdir)/src
	rm -rf $(outdir)/platform
	rm -rf $(outdir)/test


clobber:
	rm -rf $(outdir)/*
	rm -f $(mozillaroot)/js/src/configure


# ---------------------------------------------------------------OLD
# XXX Remove me!
#temp: $(v8monkeytarget) temp.cpp
	#$(CXX) -o temp temp.cpp -std=c++0x -I $(depheaders) -Wl,-L$(outdir) -Wl,-rpath=$(outdir) -l$(v8lib)
