.SUFFIXES: .pc .cpp .c .o

CC=gcc
CXX=g++

MAKEMAKE=src/mm
MYCFLAGS=-DGNU_READLINE -DDEBUG_PRT -g3 -Wall -I./build/readline/include

SRCS=$(wildcard src/*.cpp)
#// babysitter_utils.cpp comb_process.cpp bee.o test.cpp
OBJS=$(patsubst %.cpp, %.o, $(SRCS))
#//babysitter_utils.o comb_process.o bee.o test.o
EXES=$(patsubst src/bin/%.cpp.o, %.o, $(wildcard src/bin/*.cpp))

# For generating makefile dependencies..
SHELL=/bin/sh

CPPFLAGS=$(MYCFLAGS) $(OS_DEFINES)
CFLAGS=$(MYCFLAGS) $(OS_DEFINES)

ALLLDFLAGS= $(LDFLAGS)

COMMONLIBS=-Lbuild/readline/lib/ -lstdc++ -lm -lreadline -lhistory
LIBS=$(COMMONLIBS)

all: $(MAKEMAKE) $(EXES)

$(MAKEMAKE):
	@(rm -f $(MAKEMAKE))
	$(CXX) -M  $(INCLUDE) $(CPPFLAGS) $(SRCS) > $(MAKEMAKE)

$(EXES): $(OBJS) clean_bins
	@echo "Creating a executable $(subst src/,,$(subst .cpp,,$@))"
	$(CXX) -c -o $*.o $(INCLUDE) $(CPPFLAGS) -I./src $@

clean_bins:
	@(rm -rf bin/*)
	
bin/*.cpp.o: $(EXES)
	$(CXX) -o $(subst src/,,$(subst .cpp,,$@)) $(subst .cpp,.o,$@) $(OBJS) $(ALLLDFLAGS) $(LIBS)

.cpp.o: $(SRCS) $(HDR)
	$(CXX) -c -o $*.o $(INCLUDE) $(CPPFLAGS) $*.cpp

.c.o: $(SRCS) $(HDR)
	$(CC) -c $(INCLUDE) $(CFLAGS) $*.c

clean:
	rm -rf *.o src/*.o $(EXE) $(MAKEMAKE) test/*.o run_tests

clean_deps:
	rm -rf  build/*

include Tests.makefile