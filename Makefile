CC:=gcc
CFLAGS:= -I include/
DBGFLAGS:= -I include -g
BUILDDIR=build
SRCDIR=src
COMMONDIR=$(SRCDIR)/common
CLIENTDIR=$(SRCDIR)/client
SERVERDIR=$(SRCDIR)/server

CSOURCES:=$(shell find $(SRCDIR) -name '*.c')
CLIENTSRC:=$(shell find $(CLIENTDIR) -name '*.c') $(shell find $(COMMONDIR) -name '*.c')
SERVERSRC:=$(shell find $(SERVERDIR) -name '*.c') $(shell find $(COMMONDIR) -name '*.c')


HSOURCES:=$(shell find $(SRCDIR) -name '*.h')
SUBDIRS:=$(shell find $(SRCDIR) -type d)
BUILDDIRS:=$(SUBDIRS:src/%=$(BUILDDIR)/%)

OBJECTS:=$(CSOURCES:src/%.c=$(BUILDDIR)/%.o)
CLIENTOBJS:=$(CLIENTSRC:src/%.c=$(BUILDDIR)/%.o)
SERVEROBJS:=$(SERVERSRC:src/%.c=$(BUILDDIR)/%.o)

DBGOBJECTS:=$(CSOURCES:src/%.c=$(BUILDDIR)/dbg%.o)
CLIENTDBGGOBJ:=$(CLIENTSRC:src/%.c=$(BUILDDIR)/dbg%.o)
SERVERDBGOBJ:=$(SERVERSRC:src/%.c=$(BUILDDIR)/dbg%.o)

all: cchat-client cchat-server

dbg: cchat-client-dbg cchat-server-dbg

cchat-client: $(CLIENTOBJS)
	gcc -o cchat-client $(CLIENTOBJS) -lcbor -pthread

cchat-server: $(SERVEROBJS)
	gcc -o cchat-server $(SERVEROBJS) -lcbor -pthread

cchat-client-dbg: $(CLIENTDBGGOBJ)
	gcc -g -o cchat-client-dbg $(DBGOBJECTS)

cchat-server-dbg: $(SERVERDBGOBJECTS)
	gcc -g -o cchat-server-dbg $(DBGOBJECTS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIRS)
	gcc -c $(CFLAGS) $< -o $@

$(BUILDDIR)/dbg%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIRS)
	gcc -c $(DBGFLAGS) $< -o $@

watch:
	while true; do \
  	inotifywait -r -e modify $(SRCDIR) $(HSOURCES); \
    make; \
	done
clean:
	rm -rf $(BUILDDIR)
	rm -f cchat-client
	rm -f cchat-server
	rm -f cchat-server-dbg
	rm -f cchat-client-dbg

