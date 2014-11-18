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


.PHONY: all echoname clean clobber


# Directory containing upstream code
mozillaroot = deps/mozilla_upstream


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


# Name of our v8monkey library
v8monkeylibrary = libv8monkey-$(smfullversion).so


# Where shall we place our artifacts?
# XXX Is there an idiomatic name for this in make?
OUTDIR ?= dist


# Where should we build SpiderMonkey in the output dir?
DEPSBUILD ?= deps
depsout = $(OUTDIR)/$(DEPSBUILD)
depheaders = $(depsout)/dist/include
deplib = $(depsout)/dist/lib


# Naturally, our default goal is to build the v8monkey shared library
all: $(OUTDIR)/$(v8monkeylibrary)


# Enable all warnings, position-independent code and C++11 when compiling the individual object files
# Note the use of -i system to treat the JS headers as "system" headers, which turns off warning spew from those files
# XXX See if that is still required once we have broken the JSAPI dependence noted below
CXXFLAGS +=-Wall -Wextra -fPIC -isystem $(depheaders) -std=c++11 -c


# Look for cpp files in src/
vpath %.cpp src


# Look for shared libraries in dist
# XXX Will likely become redundant if we go down the "build your own damn SpiderMonkey lib" route noted at top)
vpath %.so $(OUTDIR) $(deplib)


# The files that compose libv8monkey
files := isolate init version
sources := $(addsuffix .cpp, $(files))
objects := $(addprefix $(OUTDIR)/, $(addsuffix .o, $(files)))


# isolate.o and init.o additionally depend on init.h
$(OUTDIR)/isolate.o $(OUTDIR)/init.o: src/init.h


# isolate.o and init.o depend on the JSAPI header
$(OUTDIR)/isolate.o $(OUTDIR)/init.o: $(depheaders)/jsapi.h


# version.cpp needs SMVERSION defined
$(OUTDIR)/version.o: CXXFLAGS += -DSMVERSION='"$(smfullversion)"'


# Originally, I declared a vpath for %.o to be dist, however, it is my understanding that this messes up implicit
# rules: i.e it would take the stem as-for example-dist/isolate rather than isolate and look for a prereq of
# dist/isolate.cpp rather than using the vpath for cpp files above (or rather it would look for src/dist/isolate.cpp)
# TODO Find a make guru and check if that is correct
$(OUTDIR)/%.o: %.cpp include/v8.h
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<


# Our default target is the shared library
$(OUTDIR)/$(v8monkeylibrary): $(objects) $(OUTDIR)/include/v8.h $(smlibrary)
	$(CXX) -shared -Wl,-soname,$(v8monkeylibrary) -Wl,-L$(deplib) -Wl,-R$(deplib) -o \
			$(OUTDIR)/$(v8monkeylibrary) $(OUTDIR)/*.o -l$(smlinklib)


# XXX Need to break the dependence of the include header with JSAPI.h
$(OUTDIR)/include/v8.h: include/v8.h
	mkdir -p $(OUTDIR)/include
	cp include/v8.h $(OUTDIR)/include


# XXX Need to break the dependence of the include header with JSAPI.h
include/v8.h: $(depsout)/dist/include/jsapi.h


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
	cd $(depsout) && ../../$(mozillaroot)/js/src/configure


# A config.status script is created by running configure
$(depsout)/config.status: $(mozillaroot)/js/src/configure
	mkdir -p $(depsout)
	cd $(depsout) && ../../$(mozillaroot)/js/src/configure


# To run configure we must first invoke autoconf
$(mozillaroot)/js/src/configure:
	cd $(mozillaroot)/js/src && autoconf


clean:
	rm -rf $(OUTDIR)/include
	rm -f $(OUTDIR)/*.o
	rm -f $(OUTDIR)/*.so


clobber:
	rm -rf $(OUTDIR)/*
	rm -f $(mozillaroot)/js/src/configure
