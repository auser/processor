.SUFFIXES: .pc .cpp .c .o

CC=gcc
CXX=g++

MAKEMAKE=src/mm
MYCFLAGS=-DGNU_READLINE -DDEBUG_PRT -g3 -Wall -Ibuild/readline/include

SRCS=$(wildcard src/*.cpp)
#// babysitter_utils.cpp comb_process.cpp bee.o test.cpp
OBJS=$(patsubst %.cpp, %.o, $(SRCS))
#//babysitter_utils.o comb_process.o bee.o test.o
EXE=bin/comb_test

# For generating makefile dependencies..
SHELL=/bin/sh

CPPFLAGS=$(MYCFLAGS) $(OS_DEFINES)
CFLAGS=$(MYCFLAGS) $(OS_DEFINES)

ALLLDFLAGS= $(LDFLAGS)

COMMONLIBS=-Lbuild/readline/lib/ -lstdc++ -lm -lreadline -lhistory
LIBS=$(COMMONLIBS)

all: $(MAKEMAKE) $(EXE)

$(MAKEMAKE):
	@(rm -f $(MAKEMAKE))
	$(CXX) -M  $(INCLUDE) $(CPPFLAGS) $(SRCS) > $(MAKEMAKE)

$(EXE): $(OBJS) $(LIBRARY)
	@echo "Creating a executable "
	$(CC) -o $(EXE) $(OBJS) $(ALLLDFLAGS) $(LIBS)
	
.cpp.o: $(SRCS) $(HDR)
	$(CXX) -c -o $*.o $(INCLUDE) $(CPPFLAGS) $*.cpp

.c.o: $(SRCS) $(HDR)
	$(CC) -c $(INCLUDE) $(CFLAGS) $*.c

clean:
	rm -rf *.o src/*.o $(EXE) $(MAKEMAKE) test/*.o run_tests

clean_deps:
	rm -rf  build/*

include Tests.makefile