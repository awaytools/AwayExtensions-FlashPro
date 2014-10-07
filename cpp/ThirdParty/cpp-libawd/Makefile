BUILDVERSION:=$(warning Getting build number from Mercurial)$(shell hg tip |head -n1 |cut -d : -f2 |cut -b4-)$(shell date -u "+%m%d")

CXX=g++
CC=gcc
LD=ld
CXX_SRC=$(wildcard src/*.cc)
C_SRC=$(wildcard lib/lzma/*.c lib/zlib/*.c)
OBJ=$(CXX_SRC:.cc=.o) $(C_SRC:.c=.o)
LDFLAGS=-lz
CFLAGS=-Wall -g -L. -arch i386 -arch x86_64 -DAWD_VERSION_BUILD=$(BUILDVERSION)
INCLUDE=-Iinclude -Ilib/lzma/ -Ilib/zlib/
DEFINES=-D_7ZIP_ST
LIBVER=1.0
DYLIB=libawd.dylib
STATLIB=libawd.a
PREFIX=/usr/local
LIBDIR=$(PREFIX)/lib
INCDIR=$(PREFIX)/include
BUILDDIR=build

all: dynamic

test: 
	$(CXX) -dynamic -lawd -o test test.cc
    

install: dynamic 
	cp $(BUILDDIR)/$(DYLIB) $(LIBDIR)/$(DYLIB)
	rm -rf $(INCDIR)/awd
	mkdir -p $(INCDIR)/awd
	cp -f $(BUILDDIR)/awd/* $(INCDIR)/awd/
    

dynamic: $(OBJ) 
	mkdir -p $(BUILDDIR)/awd/
	$(CXX) -dynamiclib -install_name $(LIBDIR)/$(DYLIB) -current_version $(LIBVER) $(LDFLAGS) $(CFLAGS) $(OBJ) -o $(BUILDDIR)/$(DYLIB)
	cp include/*.h $(BUILDDIR)/awd/

static: $(OBJ) 
	ar crs $(STATLIB) $(OBJ)

.c.o:
	@echo "$< > $@"
	@$(CC) -c $(INCLUDE) $(CFLAGS) $(DEFINES) $< -o $@

.cc.o:
	@echo "$< > $@"
	@$(CXX) -c $(INCLUDE) $(CFLAGS) $(DEFINES) $< -o $@
	
clean: clean-objects
	rm -rf $(BUILDDIR)/*

clean-objects:
	rm -f $(OBJ)
