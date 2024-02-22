CC:=gcc
CFLAGS:= -I include/
BUILDDIR=build
SRCDIR=src

CSOURCES:=$(shell find $(SRCDIR) -name '*.c')
OBJECTS:=$(CSOURCES:src/%.c=$(BUILDDIR)/%.o)

cchat: $(OBJECTS)
	gcc -o cchat $(OBJECTS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p build
	gcc -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)
	rm -f cchat

