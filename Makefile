#//*****************************************************************
#// Copyright policy is GNU/GPL and it is requested that
#// you include author's name and email on all copies
#// Author : Al Dev Email: alavoor@yahoo.com
#//*****************************************************************

.SUFFIXES: .pc .cpp .c .o

CC=gcc
CXX=g++

MAKEMAKE=src/mm
MYCFLAGS=-DGNU_READLINE -DDEBUG_PRT -g3 -Wall

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

COMMONLIBS=-lstdc++ -lm -lreadline
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
	rm -f *.o src/*.o $(EXE) $(MAKEMAKE)

#%.d: %.c
#       @echo "Generating the dependency file *.d from *.c"
#       $(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< | sed '\''s/$*.o/& $@/g'\'' > $@'
#%.d: %.cpp
#       @echo "Generating the dependency file *.d from *.cpp"
#       $(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< | sed '\''s/$*.o/& $@/g'\'' > $@'

#include $(SRCS:.cpp=.d)
#include $(SRCS:.c=.d)