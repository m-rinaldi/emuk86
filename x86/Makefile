module := $(shell basename $(shell pwd))
module-object := $(module:%=%.o)

.PHONY: all
all: $(module-object)
include ../common.mk
includes := ../include/

$(module-object): $(objects)
	@echo -n generating $@...
	@$(LINK.o) -Wl,--relocatable $(objects) -o $@
	@echo done

