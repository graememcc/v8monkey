# At the moment, this is a minimum viable Makefile. It makes no pretence of generality or portability, and simply
# provides enough machinery to build Spidermonkey on my Ubuntu box.


# Obviously we won't want to hard-code the name of the shared library. Leaving aside the portability issues, updating
# this every time we pull in a new upstream release will be tedious. In the Mozilla source tree, there is some
# functionality to programmatically discover the version number (config/milestone.pl) which js/src/configure.in uses
# to name the compiled library files. How can we leverage this within this Makefile? (Given such a name is dynamic,
# would that require that the Makefile is generated from a Makefile.in by autotools?)
#
# It seems to me that we likely want a way to say "Hey, don't build the Spidermonkey libs, I have them lying around
# already at ... "
# 
# Of course, at time of writing, I also need to investigate what node actually expects in terms of v8 libraries, which
# in turn leads to questions regarding the scope of my v8monkey: do we want it to purely satisfy node's v8 API
# requirements, or other embedders too?


.PHONY: all clean clobber


all: dist/libv8monkey-33.so

SMDEPS=dist/deps/dist/lib/libmozjs-33.so dist/deps/dist/include/jsapi.h


# Note the use of -i system to treat the JS headers as "system" headers, which turns off warning spew from those files
SHAREDFLAGS=-Wall -Wextra -fPIC -isystem dist/deps/dist/include -std=c++11 -c


# Generate shared lib from object files, need to link with libmozjs-33.so
dist/libv8monkey-33.so: dist/isolate.o dist/init.o
	$(CXX) -shared -Wl,-soname,libv8monkey-33.so -Wl,-Ldist/deps/dist/lib -Wl,-Rdist/deps/dist/lib -o dist/libv8monkey-33.so dist/*.o -lmozjs-33


dist/isolate.o: $(SMDEPS) src/init.h src/isolate.cpp
	$(CXX) $(SHAREDFLAGS) -o dist/isolate.o src/isolate.cpp


dist/init.o: $(SMDEPS) src/init.h src/init.cpp
	$(CXX) $(SHAREDFLAGS) -o dist/init.o src/init.cpp


$(SMDEPS): dist/deps/Makefile
	$(MAKE) -C dist/deps


dist/deps/Makefile: dist/deps/config.status
	cd dist/deps && ../../deps/mozilla_upstream/js/src/configure


dist/deps/config.status: deps/mozilla_upstream/js/src/configure
	mkdir -p dist/deps
	cd dist/deps && ../../deps/mozilla_upstream/js/src/configure


deps/mozilla_upstream/js/src/configure:
	cd deps/mozilla_upstream/js/src && autoconf


clean:
	rm -rf dist/*


clobber:
	rm -rf dist/*
	rm deps/mozilla_upstream/js/src/configure
