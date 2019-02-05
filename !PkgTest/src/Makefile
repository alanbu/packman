# Makefile for PkgTest

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

  TARGET=^.!RunImage
else
  TARGET=../!RunImage,ff8
endif

CPPFLAGS= -ITBX: -ILibPKG: -ILibCurl: -Iluacc
CXXFLAGS = -O2 -std=c++11 -Wall -mpoke-function-name -mthrowback

LD = g++
LDFLAGS = -LTBX: -ltbx  -LLibPkg: -lpkg -LLibCurl: -lcurl -lssh2 -lssl -lcrypto -LZLib: -lz -Lluacc -lluacc -lstdc++ -static

CCSRC = $(wildcard *.cc)
OBJECTS = $(CCSRC:.cc=.o)

$(TARGET):	!RunImageElf
	elf2aif !RunImageElf $(TARGET)
!RunImageElf:	$(OBJECTS)
	$(LD) $(LDFLAGS) -o !RunImageElf $(OBJECTS)

ifeq ($(HOST),riscos)
include $(CCSRC:.cc=.d)
endif

