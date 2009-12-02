# Makefile for PackMan

include FixDeps:Rules/make

CXX=g++
CXXFLAGS = -O2 -Wall -mpoke-function-name -mthrowback -ITBX: -ILibPKG: -ILibCurl:

LD = g++
LDFLAGS = -LTBX: -ltbx -LLibPkg: -lpkg -LRTK: -lrtk -LLibCurl: -lcurl -LZLib: -lz -lstdc++ -static

TARGET=!PackMan/!RunImage

CCSRC = $(wildcard src/*.cc)
OBJECTS = $(CCSRC:.cc=.o)

all:	$(TARGET)

$(TARGET): packman
	elf2aif packman $(TARGET)

packman:	$(OBJECTS)
	$(LD) $(LDFLAGS) -o packman $(OBJECTS)

#include $(CCSRC:.cc=.d)
