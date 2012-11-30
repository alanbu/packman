# Makefile for PackMan

# Detect host
HOST := $(shell uname -s)
ifeq ($(HOST),)
  # Assume RISC OS
  HOST := riscos
else
  ifeq ($(HOST),RISC OS)
    HOST := riscos
  endif
endif

ifeq ($(HOST),riscos)
  include FixDeps:Rules/make
endif

CXX=g++
CXXFLAGS = -O2 -Wall -mpoke-function-name -mthrowback -ITBX: -ILibPKG: -ILibCurl:

LD = g++
LDFLAGS = -LTBX: -ltbx -LLibPkg: -lpkg -LRTK: -lrtk -LLibCurl: -lcurl -LZLib: -lz -lstdc++ -static

ifeq ($(HOST),riscos)
  TARGET=!PackMan.!RunImage
else
  TARGET=!PackMan/!RunImage,ff8
endif

CCSRC = $(wildcard src/*.cc)
OBJECTS = $(CCSRC:.cc=.o)

all:	$(TARGET)

$(TARGET): packman
	elf2aif packman $(TARGET)

packman:	$(OBJECTS)
	$(LD) $(LDFLAGS) -o packman $(OBJECTS)

#include $(CCSRC:.cc=.d)

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
