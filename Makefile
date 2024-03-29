CC:=gcc
CFLAGS:= -I include/ -g
BUILDDIR=build
SRCDIR=src

CSOURCES:=$(shell find $(SRCDIR) -name '*.c')
HSOURCES:=$(shell find $(SRCDIR) -name '*.h')
SUBDIRS:=$(shell find $(SRCDIR) -type d)
BUILDDIRS:=$(SUBDIRS:src/%=$(BUILDDIR)/%)
OBJECTS:=$(CSOURCES:src/%.c=$(BUILDDIR)/%.o)
DBGOBJECTS:=$(CSOURCES:src/%.c=$(BUILDDIR)/dbg%.o)

cchat: $(OBJECTS)
	gcc -o cchat $(OBJECTS) -lcbor -pthread

cchat-dbg: $(DBGOBJECTS)
	gcc -g -o cchat-dbg $(DBGOBJECTS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIRS)
	gcc -c $(CFLAGS) $< -o $@

$(BUILDDIR)/dbg%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIRS)
	gcc -c $(CFLAGS) $< -o $@

watch:
	while true; do \
  	inotifywait -r -e modify $(SRCDIR) $(HSOURCES); \
    make; \
	done
clean:
	rm -rf $(BUILDDIR)
	rm -f cchat
	rm -f cchat-dbg

