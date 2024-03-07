CC:=gcc
CFLAGS:= -I include/
BUILDDIR=build
SRCDIR=src

CSOURCES:=$(shell find $(SRCDIR) -name '*.c')
SUBDIRS:=$(shell find $(SRCDIR) -type d)
BUILDDIRS:=$(SUBDIRS:src/%=$(BUILDDIR)/%)
OBJECTS:=$(CSOURCES:src/%.c=$(BUILDDIR)/%.o)
DBGOBJECTS:=$(CSOURCES:src/%.c=$(BUILDDIR)/dbg%.o)

cchat: $(OBJECTS)
	gcc -o cchat $(OBJECTS) -l cbor -g

cchat-dbg: $(DBGOBJECTS)
	gcc -g -o cchat-dbg $(DBGOBJECTS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIRS)
	gcc -c $(CFLAGS) $< -o $@

$(BUILDDIR)/dbg%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIRS)
	gcc -c $(CFLAGS) -g $< -o $@

clean:
	rm -rf $(BUILDDIR)
	rm -f cchat
	rm -f cchat-dbg

