module := $(shell basename $(shell pwd))
module-object := $(module:%=%.o)

.PHONY: all
all: $(module-object)
include ../common.mk
includes := ../include/

$(module-object): $(objects)
	@echo -n generating $@...
	@$(LD) --relocatable $(objects) -o $@
	@echo done

