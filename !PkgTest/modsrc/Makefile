# Probably the smallest example of how to create a module using GCCSDK.

CC          = gcc
CCFLAGS     = -mthrowback -mmodule -O3
ASM         = $(CC)
ASMFLAGS    = $(CCFLAGS)
LINK        = gcc
LINKFLAGS   = -mmodule
CMHG        = cmunge
CMHGFLAGS   = -tgcc -32bit

OBJS100 = testpkgmodule100.o module100.o
OBJS120 = testpkgmodule120.o module120.o
OBJS122 = testpkgmodule122.o module122.o

all: testpkgmodule100 testpkgmodule120 testpkgmodule122

testpkgmodule100: $(OBJS100)
	$(LINK) $(LINKFLAGS) $(OBJS100) -o $@

testpkgmodule120: $(OBJS120)
	$(LINK) $(LINKFLAGS) $(OBJS120) -o $@

testpkgmodule122: $(OBJS122)
	$(LINK) $(LINKFLAGS) $(OBJS122) -o $@


.SUFFIXES:
.SUFFIXES: .c .o .h .cmhg

.c.o:;	$(CC) $(CCFLAGS) -c -o $@ $<

# CMunge invocation rule:
.cmhg.h:;	$(CMHG) $(CMHGFLAGS) -d $@ $<
.cmhg.o:;	$(CMHG) $(CMHGFLAGS) -o $@ $<

# Dependencies:
testpkgmodule100.c: module100.h
testpkgmodule120.c: module120.h
testpkgmodule122.c: module122.h
