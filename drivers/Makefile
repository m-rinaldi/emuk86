module := $(shell basename $(shell pwd))
module-object := $(module:%=%.o)

.PHONY: all
all: $(module-object)
include ../common.mk
includes := ../include/

indentation := $(shell perl -e 'print "   " x ${MAKELEVEL}')

$(module-object): $(objects) $(firstword $(MAKEFILE_LIST))
	@echo -n "$(indentation)generating $@..."
	@$(LINK.o) -Wl,--relocatable $(objects) -o $@
	@$(echo-ok)

