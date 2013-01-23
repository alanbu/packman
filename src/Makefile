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
CPPFLAGS= -ITBX: -ILibPKG: -ILibCurl:
CXXFLAGS= -O2 -Wall -mpoke-function-name -mthrowback

LD = g++
LDFLAGS = -LTBX: -ltbx -LLibPkg: -lpkg -LLibCurl: -lcurl -LZLib: -lz -lstdc++ -static

ifeq ($(HOST),riscos)
  TARGET=^.!PackMan.!RunImage
else
  TARGET=../!PackMan/!RunImage,ff8
endif

CCSRC = $(wildcard *.cc)
OBJECTS = $(CCSRC:.cc=.o)

all:	$(TARGET)

$(TARGET): packman
	elf2aif packman $(TARGET)

packman:	$(OBJECTS)
	$(LD) $(LDFLAGS) -o packman $(OBJECTS)

ifeq ($(HOST),riscos)
include $(CCSRC:.cc=.d)
endif

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
