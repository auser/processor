#//*****************************************************************
#// Copyright policy is GNU/GPL and it is requested that
#// you include author's name and email on all copies
#// Author : Al Dev Email: alavoor@yahoo.com
#//*****************************************************************

.SUFFIXES: .pc .cpp .c .o

HOSTFLAG=-DLinux
#HOSTFLAG=-DSunOS

CC=gcc
CXX=g++

MAKEMAKE=mm
MYCFLAGS=-DDEBUG_PRT -g3 -Wall

SRCS=babysitter_utils.cpp comb_process.cpp test.cpp
OBJS=babysitter_utils.o comb_process.o test.o
EXE=test_proc

# For generating makefile dependencies..
SHELL=/bin/sh

CPPFLAGS=$(MYCFLAGS) $(OS_DEFINES)
CFLAGS=$(MYCFLAGS) $(OS_DEFINES)

ALLLDFLAGS= $(LDFLAGS)

COMMONLIBS=-lstdc++ -lm
LIBS=$(COMMONLIBS)

all: $(MAKEMAKE) $(EXE)

$(MAKEMAKE):
	@(rm -f $(MAKEMAKE))
	$(PURIFY) $(CXX) -M  $(INCLUDE) $(CPPFLAGS) *.cpp > $(MAKEMAKE)

$(EXE): $(OBJS) $(LIBRARY)
	@echo "Creating a executable "
	$(CC) -o $(EXE) $(OBJS) $(ALLLDFLAGS) $(LIBS)
	
.cpp.o: $(SRCS) $(HDR)
	$(PURIFY) $(CXX) -c  $(INCLUDE) $(HOSTFLAG) $(CPPFLAGS) $*.cpp

.c.o: $(SRCS) $(HDR)
	$(PURIFY) $(CC) -c $(INCLUDE) $(HOSTFLAG) $(CFLAGS) $*.c

clean:
	rm -f *.o
	rm -f $(EXE)
	rm -f $(MAKEMAKE)

#%.d: %.c
#       @echo "Generating the dependency file *.d from *.c"
#       $(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< | sed '\''s/$*.o/& $@/g'\'' > $@'
#%.d: %.cpp
#       @echo "Generating the dependency file *.d from *.cpp"
#       $(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< | sed '\''s/$*.o/& $@/g'\'' > $@'

# Must include all the c flags for -M option
#$(MAKEMAKE):
#       @echo "Generating the dependency file *.d from *.cpp"
#       $(CXX) -M  $(INCLUDE) $(CPPFLAGS) *.cpp > $(MAKEMAKE)

include $(MAKEMAKE)
#include $(SRCS:.cpp=.d)
#include $(SRCS:.c=.d)