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


#**********************************************************************************************************************#
#                                                      Variables                                                       #
#**********************************************************************************************************************#


# Where does our copy of the mozilla-release codebase live?
mozillaroot = $(CURDIR)/deps/mozilla


# There's a perl script in the Mozilla source tree we can use to programatically discover the current version
milestonecommand = $(mozillaroot)/config/milestone.pl


# Lets put the above command to use. The name of the SpiderMonkey library has the major version number as its suffix
smmajorversion = $(shell $(milestonecommand) --topsrcdir=$(mozillaroot) --symbolversion)


# Function for calculating library filenames
libname = $(addprefix lib, $(addsuffix .so, $(strip $(1))))


# What V8 API are we emulating?
v8compat = 3.28.73.0


# Our own V8Monkey library will have a suffix which denotes the full SpiderMonkey version we were built from
smfullversion = $(shell $(milestonecommand) --topsrcdir=$(mozillaroot))


# Compute the name of the SpiderMonkey library that we can pass as a -l flag to the linker
smlib = mozjs-$(smmajorversion)


# Likewise, compute the name of our library for linking purposes
v8lib = v8monkey-$(smfullversion)


# We will also build a special version of the library for testing our internals. This version of the library will
# export additional symbols to aid testing. Yes, yes, I know, one should simply test the interface, but there's enough
# gnarly bookkeeping involved that I need to seek additional comfort
v8testlib = v8monkey-test-$(smfullversion)


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


# What is the path of the JSAPI header
JSAPIheader = $(smheadersdir)/jsapi.h


# What directory will the SpiderMonkey library be in after building?
smlibdir = $(depsdir)/dist/lib


# Absolute filename of the SpiderMonkey shared library
smtarget = $(smlibdir)/$(call libname, $(smlib))


# Where will we write the V8 header?
v8monkeyheadersdir = $(outdir)/include


# The V8 header (for dependency purposes)
v8monkeyheader = $(v8monkeyheadersdir)/v8.h


# Absolute filename of the V8Monkey library
v8monkeytarget = $(outdir)/$(call libname, $(v8lib))


# Directory where the "internal" library object files will be built
v8monkeyinternaldir = $(outdir)/test/internalLib


# Absolute filename of the internal test variant of the V8Monkey library
v8monkeytesttarget = $(v8monkeyinternaldir)/$(call libname, $(v8testlib))


# Location of API test files
apitestbase = $(outdir)/test/api


# Location of internal test files
internaltestbase = $(outdir)/test/internal


#**********************************************************************************************************************#
#                                                     Compilation                                                      #
#**********************************************************************************************************************#


# We will generally need headers from 3 places:
#  - We will often need the JSAPI header
#  - We will often need the V8 header
#  - some files in src/ may depend on additional headers in src
# Note: we use -isystem to prevent warning spew from JSAPI
# TODO: Is there a idiomatic name for this in makefiles?
includedirs = $(v8monkeyheadersdir) $(CURDIR)/src
includeopt = $(addprefix -I, $(includedirs))
includeopt += -isystem $(smheadersdir)


# What warnings do we want to enable?
# TODO: Is there an idiomatic name for this in makefiles?
# TODO: Do we want to allow the user to specify additional warnings? i.e. should this be additive?
warningswitches = all extra init-self switch-default switch-enum unused  strict-overflow=4 shadow \
                  unsafe-loop-optimizations cast-qual cast-align conversion sign-conversion logical-op \
                  packed redundant-decls unreachable-code inline abi ctor-dtor-privacy non-virtual-dtor \
                  old-style-cast overloaded-virtual sign-promo missing-include-dirs
warnings = $(addprefix -W, $(warningswitches))


# Flags to pass to the C++ compiler:
#   - std=c++0x SpiderMonkey uses some C++11 features in the header
#   - fvisibility=hidden We want to minimize the number of symbols exported
#   - fPIC shared libraries need position-independent code
#   - DV8SHARED=1 -DBUILDING_V8_SHARED preprocess the header correctly
# XXX Remove dependency flag (-MMD)
# XXX Remove debug flag (-g)
# XXX Remove -DDEBUG
# XXX Remove suggest
# XXX Remove conversion
CXXFLAGS += -MMD -pedantic -Wsuggest-attribute=const -g -DV8_SHARED=1 -DBUILDING_V8_SHARED=1 -DDEBUG=1 $(warnings) \
            $(includeopt) -fPIC -fvisibility=hidden -fstrict-aliasing -std=c++0x


# Define a command that will produce a link command for the given library name
linkcommand = -Wl,-L$(strip $(1)) -Wl,-rpath=$(strip $(1)) -l$(strip $(2))


# Because we build normal and "internal" versions of the object files, expressing dependencies could become
# a bit of a chore
variants = $(outdir)/$(strip $(1)).o $(v8monkeyinternaldir)/$(strip $(1)).o


# A useful general rule for building files
# This rule will catch everything except the "internals lib" versions of the source files
# XXX Does this really do what I think it does?
$(outdir)/%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<


# Rule for headers: primarily to capture transitive header-only dependencies
include/%.h src/%.h:
	touch $@


# Certain files need to be aware of the SpiderMonkey version
$(call variants, src/engine/version) $(outdir)/test/api/test_version.o: CXXFLAGS += -DV8COMPAT='"$(v8compat)"' -DSMVERSION='"$(smfullversion)"'


#**********************************************************************************************************************#
#                                                       Sources                                                        #
#**********************************************************************************************************************#


# The V8Monkey library is composed of the following files
enginestems = $(addprefix src/engine/, init version)
engineobjects = $(addsuffix .o, $(enginestems))

platformstems = $(addprefix src/platform/, platform)
platformobjects = $(addsuffix .o, $(platformstems))

runtimestems = $(addprefix src/runtime/, IsolateAPI isolate handlescope persistent)
runtimeobjects = $(addsuffix .o, $(runtimestems))

threadstems = $(addprefix src/threads/, locker)
threadobjects = $(addsuffix .o, $(threadstems))

typestems = $(addprefix src/types/, number primitives value v8monkeyobject)
typeobjects = $(addsuffix .o, $(typestems))

utilsstems = $(addprefix src/utils/, SpiderMonkeyUtils)
utilsobjects = $(addsuffix .o, $(utilsstems))

allstems = $(enginestems) $(platformstems) $(runtimestems) $(threadstems) $(typestems) $(utilsstems)
allobjects = $(engineobjects) $(platformobjects) $(runtimeobjects) $(threadobjects) $(typeobjects) $(utilsobjects)

v8sources = $(addsuffix .cpp, $(allstems))
v8objects = $(addprefix $(outdir)/, $(allobjects))

# Extra files for the internal test version of the library
testlibobjects = $(addprefix $(v8monkeyinternaldir)/, $(allobjects))


# Names of the test harness executables: we need these for the all target
testharnesses = run_v8monkey_tests run_v8monkey_internal_tests
testsuites = $(addprefix $(outdir)/test/, $(testharnesses))


#**********************************************************************************************************************#
#                                                  Output directories                                                  #
#**********************************************************************************************************************#


# Here we list all the output directories that will be created in dist


# Where are object files placed when we build them in their various guises?
srctree = engine platform runtime threads types utils
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
$(addprefix $(outdir)/, $(platformobjects)): | $(outdir)/src/platform
$(addprefix $(outdir)/, $(runtimeobjects)): | $(outdir)/src/runtime
$(addprefix $(outdir)/, $(threadobjects)): | $(outdir)/src/threads
$(addprefix $(outdir)/, $(typeobjects)): | $(outdir)/src/types
$(addprefix $(outdir)/, $(utilsobjects)): | $(outdir)/src/utils
$(addprefix $(v8monkeyinternaldir)/, $(engineobjects)): | $(v8monkeyinternaldir)/src/engine
$(addprefix $(v8monkeyinternaldir)/, $(platformobjects)): | $(v8monkeyinternaldir)/src/platform
$(addprefix $(v8monkeyinternaldir)/, $(runtimeobjects)): | $(v8monkeyinternaldir)/src/runtime
$(addprefix $(v8monkeyinternaldir)/, $(threadobjects)): | $(v8monkeyinternaldir)/src/threads
$(addprefix $(v8monkeyinternaldir)/, $(typeobjects)): | $(v8monkeyinternaldir)/src/types
$(addprefix $(v8monkeyinternaldir)/, $(utilsobjects)): | $(v8monkeyinternaldir)/src/utils


#**********************************************************************************************************************#
#                                                        Goals                                                         #
#**********************************************************************************************************************#


.PHONY: all clean clobber check make_sm valgrind valgrind-mem-api valgrind-mem-int


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
	rm -rf $(v8monkeyheadersdir)
	rm -rf $(outdir)/src
	rm -rf $(outdir)/test


clobber:
	rm -rf $(outdir)/*
	rm -f $(mozillaroot)/js/src/configure


valgrind-mem-api: $(testsuites)
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --leak-check-heuristics=stdstring --log-file=mem_api --trace-children=yes --gen-suppressions=all $(outdir)/test/run_v8monkey_tests

valgrind-mem-int: $(testsuites)
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --leak-check-heuristics=stdstring --log-file=mem_internal --trace-children=yes  --suppressions=$(CURDIR)/int_suppress.supp --gen-suppressions=all $(outdir)/test/run_v8monkey_internal_tests


valgrind: $(testsuites) valgrind-mem-api valgrind-mem-int
	valgrind --tool=helgrind --log-file=thread_api --read-var-info=yes --trace-children=yes $(outdir)/test/run_v8monkey_tests
	valgrind --tool=helgrind --log-file=thread_internal --read-var-info=yes --trace-children=yes $(outdir)/test/run_v8monkey_internal_tests


# ---------------------------------------------------------------OLD
# XXX Remove me!
temp: $(v8monkeytarget) temp.cpp
	$(CXX) -v -Wl,-v -g -o temp temp.cpp -std=c++0x -I $(v8monkeyheadersdir) $(call linkcommand, $(outdir), $(v8lib)) -lpthread


# TODO Provide a target or flag for debug builds. We need to ensure this builds a debug SpiderMonkey library too


#**********************************************************************************************************************#
#                                                       Includes                                                       #
#**********************************************************************************************************************#

v8monkeyheaders = $(addsuffix .h, $(addprefix $(v8monkeyheadersdir)/, v8 v8config v8stdint))


$(v8monkeyheadersdir)/%.h: include/%.h | $(v8monkeyheadersdir)
	cp $< $(@D)


$(v8monkeyheadersdir)/v8stdint.h: $(v8monkeyheadersdir)/v8config.h


$(v8monkeyheader): $(v8monkeyheadersdir)/v8stdint.h


#**********************************************************************************************************************#
#                                                       V8Monkey                                                       #
#**********************************************************************************************************************#


# Our default goal is to build the v8monkey shared library, and the testsuites
# XXX Remove ctags
all: $(v8monkeytarget) $(testsuites)
	/usr/local/bin/ctags -R src test include


# The main task is building the shared library
$(v8monkeytarget): $(v8objects) $(v8monkeyheaders) $(smtarget)
	@echo && \
	echo "********************************************************************************" && \
	echo "*                                                                              *" && \
	echo "*                               Linking V8Monkey                               *" && \
	echo "*                                                                              *" && \
	echo "********************************************************************************" && \
	echo
	$(CXX) -shared -o $@ $(v8objects) $(call linkcommand, $(smlibdir), $(smlib)) $(v8platformtarget)


#**********************************************************************************************************************#
#                                              V8Monkey file dependencies                                              #
#**********************************************************************************************************************#

# XXX I think the objects should depend on the local h file

$(call variants, src/engine/init): $(v8monkeyheader) src/runtime/isolate.h src/platform/platform.h src/utils/test.h \
                                   src/utils/V8MonkeyCommon.h src/utils/SpiderMonkeyUtils.h $(JSAPIheader)


$(call variants, src/engine/version): $(v8monkeyheader)


src/platform/platform.h: src/utils/test.h src/utils/V8MonkeyCommon.h


$(call variants, src/platform/platform): src/platform/platform.h src/utils/V8MonkeyCommon.h


$(call variants, src/runtime/handlescope): $(v8monkeyheader) src/types/objectblock.h \
                                           src/runtime/isolate.h src/types/base_types.h src/utils/V8MonkeyCommon.h


$(call variants, src/runtime/IsolateAPI): $(v8monkeyheader) src/runtime/isolate.h


$(call variants, src/runtime/isolate): $(v8monkeyheader) src/data_structures/destruct_list.h \
                                       src/types/objectblock.h src/runtime/isolate.h \
                                       src/utils/SpiderMonkeyUtils.h src/platform/platform.h src/utils/test.h \
                                       src/utils/V8MonkeyCommon.h $(v8monkeyheadersdir)/v8config.h \
                                       src/types/base_types.h


$(call variants, src/runtime/persistent): $(v8monkeyheader) src/types/objectblock.h \
                                           src/runtime/isolate.h src/types/base_types.h src/utils/V8MonkeyCommon.h


src/runtime/isolate.h: $(v8monkeyheader) src/platform/platform.h src/utils/test.h src/types/base_types.h


src/threads/autolock.h: src/platform/platform.h


$(call variants, src/threads/locker): $(v8monkeyheader) src/runtime/isolate.h src/utils/V8MonkeyCommon.h


src/types/base_types.h: $(v8monkeyheader) src/utils/SpiderMonkeyUtils.h src/utils/test.h


src/types/objectblock.h: src/types/base_types.h src/utils/V8MonkeyCommon.h


src/types/value_types.h: $(JSAPIheader) src/utils/test.h


$(call variants, src/types/number): $(v8monkeyheader) src/types/base_types.h src/types/value_types.h \
                                    src/utils/V8MonkeyCommon.h


$(call variants, src/types/primitives): $(v8monkeyheader) src/types/base_types.h src/types/value_types.h \
                                        src/utils/V8MonkeyCommon.h


$(call variants, src/types/value): $(v8monkeyheader) src/types/value_types.h src/utils/V8MonkeyCommon.h


$(call variants, src/types/v8monkeyobject): $(v8monkeyheader) src/types/base_types.h


src/utils/V8MonkeyCommon.h: src/utils/test.h


$(call variants, src/utils/SpiderMonkeyUtils): $(JSAPIheader) src/platform/platform.h src/utils/SpiderMonkeyUtils.h \
											   src/utils/V8MonkeyCommon.h


src/utils/SpiderMonkeyUtils.h: $(JSAPIheader) src/utils/test.h


#**********************************************************************************************************************#
#                                                      Testsuites                                                      #
#**********************************************************************************************************************#


# The API test harness is composed from the following
teststems = death handlescope init isolate locker number primitives threadID version
testfiles = $(addprefix test/api/test_, $(teststems))
testsources = $(addsuffix .cpp, $(testfiles))
testobjects = $(addprefix $(outdir)/, $(addsuffix .o, $(testfiles)))


# The "internals" test harness is composed from the following
internalteststems = death destructlist fatalerror handlescope init isolate miscutils objectblock persistent \
                    platform refcount smartpointer spidermonkeyutils threadID utf8 value
internaltestfiles = $(addprefix test/internal/test_, $(addsuffix _internal, $(internalteststems)))
internaltestsources = $(addsuffix .cpp, $(internaltestfiles))
internaltestobjects = $(addprefix $(outdir)/, $(addsuffix .o, $(internaltestfiles)))


# Test objects need to locate the V8MonkeyTest test header
$(testobjects) $(internaltestobjects) $(testsuites) $(outdir)/test/harness/V8MonkeyTest.o: CXXFLAGS += -I$(CURDIR)/test/harness


# All test files depend on the V8MonkeyTest header
$(testobjects) $(internaltestobjects) $(testsuites): test/harness/V8MonkeyTest.h


# V8MonkeyTest also depends on its output directory
$(outdir)/test/harness/V8MonkeyTest.o: $(outdir)/test/harness


#**********************************************************************************************************************#
#                                                    API Testsuite                                                     #
#**********************************************************************************************************************#


# TODO The little trick we use in the test harness to build the linked list of test functions is neat, but it is sensitive
#      to compilation order. It will segfault if the test files are specified before the V8MonkeyTest file


# Build the "standard" test harness
$(outdir)/test/run_v8monkey_tests: test/harness/run_v8monkey_tests.cpp $(testobjects) $(outdir)/test/harness/V8MonkeyTest.o \
                                   $(v8monkeytarget)
	@echo && \
	echo "********************************************************************************" && \
	echo "*                                                                              *" && \
	echo "*                          Building API Testsuite...                           *" && \
	echo "*                                                                              *" && \
	echo "********************************************************************************" && \
	echo
	$(CXX) $(CXXFLAGS) -g -o $@ test/harness/run_v8monkey_tests.cpp $(outdir)/test/harness/V8MonkeyTest.o $(testobjects) \
                        $(call linkcommand, $(outdir), $(v8lib))


# The individual object files depend on the existence of their output directory
$(testobjects): | $(outdir)/test/api


# Expands an api test stem to its object file
apitest = $(addprefix $(apitestbase)/test_, $(addsuffix .o,  $(strip $(1))))


#**********************************************************************************************************************#
#                                           API Testsuite file dependencies                                            #
#**********************************************************************************************************************#


$(call apitest, death): $(v8monkeyheader)


$(call apitest, handlescope): $(v8monkeyheader)


$(call apitest, init): $(v8monkeyheader) src/platform/platform.h src/utils/V8MonkeyCommon.h


$(call apitest, isolate): $(v8monkeyheader) src/platform/platform.h


$(call apitest, locker): $(v8monkeyheader)  src/platform/platform.h


$(call apitest, number): $(v8monkeyheader)


$(call apitest, primitives): $(v8monkeyheader)


$(call apitest, threadID): $(v8monkeyheader) src/platform/platform.h


#**********************************************************************************************************************#
#                                                 Internals Testsuite                                                  #
#**********************************************************************************************************************#


# Modify compilation of the internal library versions of V8Monkey components
$(testlibobjects) $(internaltestobjects): CXXFLAGS += -DV8MONKEY_INTERNAL_TEST=1


# TODO The directory prereq here isn't quite right
# XXX Really need to fix this. It's annoying me. Prereq for code reorg
$(outdir)/test/internalLib/%.o: %.cpp | $(outdir)/test/internalLib/src
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<


# The internal test object files depend on the existence of their output directory
$(internaltestobjects): | $(outdir)/test/internal


# TODO The test harness builds a linked list of test functions using a macro. This is a neat trick, but has the drawback
#      of being sensitive to compilation order; the resulting program will segfault when the test files are specified
#      before V8MonkeyTest.cpp.


# Build a version of the shared library for internal tests
$(v8monkeytesttarget): $(testlibobjects) $(v8monkeyheader) $(smtarget)
	@echo && \
	echo "********************************************************************************" && \
	echo "*                                                                              *" && \
	echo "*           Building custom V8Monkey library for internal testing...           *" && \
	echo "*                                                                              *" && \
	echo "********************************************************************************" && \
	echo
	$(CXX) -shared -o $@ $(testlibobjects) $(call linkcommand, $(smlibdir), $(smlib))


# Build the internal test harness
# Note: we reuse the driver file run_v8monkey_tests.cpp. That isn't a copy/paste error
$(outdir)/test/run_v8monkey_internal_tests: test/harness/run_v8monkey_tests.cpp $(internaltestobjects) \
                                            $(outdir)/test/harness/V8MonkeyTest.o $(v8monkeytesttarget)
	@echo && \
	echo "********************************************************************************" && \
	echo "*                                                                              *" && \
	echo "*                        Building Internal Testsuite...                        *" && \
	echo "*                                                                              *" && \
	echo "********************************************************************************" && \
	echo
	$(CXX) $(CXXFLAGS) -o $@ test/harness/run_v8monkey_tests.cpp $(outdir)/test/harness/V8MonkeyTest.o \
                        $(internaltestobjects) \
                        $(call linkcommand, $(outdir)/test/internalLib, $(v8testlib))


#**********************************************************************************************************************#
#                                        Internals Testsuite file dependencies                                         #
#**********************************************************************************************************************#


# Expands an internal test stem to its object file
inttest = $(addprefix $(internaltestbase)/test_, $(addsuffix _internal.o,  $(strip $(1))))


$(call inttest, death): $(v8monkeyheader) src/utils/test.h src/utils/V8MonkeyCommon.h


$(call inttest, destructlist): src/data_structures/destruct_list.h src/types/base_types.h


$(call inttest, fatalerror): $(v8monkeyheader) src/runtime/isolate.h src/utils/test.h src/utils/V8MonkeyCommon.h


$(call inttest, handlescope): $(v8monkeyheader) src/types/objectblock.h src/runtime/isolate.h \
                              src/utils/test.h src/utils/V8MonkeyCommon.h


$(call inttest, init): $(v8monkeyheader) src/platform/platform.h src/runtime/isolate.h src/utils/test.h


$(call inttest, isolate): $(v8monkeyheader) $(JSAPIheader) src/platform/platform.h src/runtime/isolate.h src/utils/test.h \
                          src/types/base_types.h src/utils/SpiderMonkeyUtils.h


$(call inttest, miscutils): src/utils/MiscUtils.h


$(call inttest, objectblock): src/types/objectblock.h


$(call inttest, platform): src/platform/platform.h


$(call inttest, persistent): $(v8monkeyheader) src/types/objectblock.h src/runtime/isolate.h \
							 src/types/base_types.h src/utils/test.h src/utils/V8MonkeyCommon.h


$(call inttest, refcount): $(v8monkeyheader) src/types/base_types.h


$(call inttest, smartpointer): src/data_structures/smart_pointer.h src/types/base_types.h


$(call inttest, spidermonkeyutils): src/utils/SpiderMonkeyUtils.h


$(call inttest, threadID): $(v8monkeyheader) src/runtime/isolate.h src/utils/test.h


$(call inttest, utf8): src/utils/Encoding.h


$(call inttest, value): $(v8monkeyheader) src/runtime/isolate.h src/utils/test.h src/utils/V8MonkeyCommon.h


#**********************************************************************************************************************#
#                                                     Spidermonkey                                                     #
#**********************************************************************************************************************#

# We always want to run the Makefile to ensure our SpiderMonkey build is up-to-date. This is performed by the make_sm
# target, which is a prerequisite of the V8Monkey library. make_sm has a prerequisite with an empty rule to ensure that
# the recipe is always invoked.


# The JSAPI header is created as a by-product of building Spidermonkey
$(JSAPIheader): $(smtarget)


# We build (and update) the library by running make
$(smtarget): make_sm
	@echo && \
	echo "#******************************************************************************#" && \
	echo "#                                                                              #" && \
	echo "#                        Finished building SpiderMonkey                        #" && \
	echo "#                                                                              #" && \
	echo "#******************************************************************************#" && \
	echo


# Running the Makefile requires it to exist
make_sm: $(depsdir)/Makefile FORCE_MAKE
	@echo && \
	echo "********************************************************************************" && \
	echo "*                                                                              *" && \
	echo "*                           Building SpiderMonkey...                           *" && \
	echo "*                                                                              *" && \
	echo "********************************************************************************" && \
	echo
	make -C $(depsdir)


FORCE_MAKE:


# To create the Makefile, we must have a config.status script
$(depsdir)/Makefile: $(depsdir)/config.status


# A config.status script is created by running configure
$(depsdir)/config.status: $(mozillaroot)/js/src/configure | $(depsdir)
	rm -f $(depsdir)/Makefile
	cd $(depsdir) && $<


# To run configure we must first invoke autoconf
# XXX For some reason, I've been finding it necessary to add declare JS_STANDALONE to avoid build errors. I believe this
#     shouldn't be necessary. YMMV.
# XXX Should the JS_STANDALONE be on autoconf or configure?
$(mozillaroot)/js/src/configure: $(mozillaroot)/js/src/configure.in
	rm -f $(smheadersdir)/jsapi.h
	cd $(mozillaroot)/js/src && JS_STANDALONE=1 autoconf
