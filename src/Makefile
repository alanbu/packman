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
CXXFLAGS= -std=c++11 -O2 -Wall -mpoke-function-name -mthrowback

LD = g++

CCRES=ccres
LDFLAGS = -LTBX: -ltbx -LLibPkg: -lpkg -LLibCurl: -lcurl -lcares -lssh2 -lssl -lcrypto -LZLib: -lz -lstdc++ -static

ifeq ($(HOST),riscos)
  TARGET=^.!PackMan.!RunImage
  TARGETRES=^.!PackMan.Res
  CCRES=<CCRes$$Dir>.ccres
else
  TARGET=../!PackMan/!RunImage,ff8
  TARGETRES=../!PackMan/Res,fae
  CCRES=$(GCCSDK_INSTALL_ENV)/bin/ccres
endif

CCSRC = $(wildcard *.cc)
OBJECTS = $(CCSRC:.cc=.o)

all:	$(TARGET) $(TARGETRES)

$(TARGET): packman
	elf2aif packman $(TARGET)

packman:	$(OBJECTS)
	$(LD) $(LDFLAGS) -o packman $(OBJECTS)

$(TARGETRES): CCRes
	$(CCRES) CCRes $(TARGETRES)

ifeq ($(HOST),riscos)
include $(CCSRC:.cc=.d)
endif

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
