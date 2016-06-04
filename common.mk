CC := ~/opt/cross/bin/i686-elf-gcc

MV := mv -f

LDFLAGS     += -nostdlib 
CFLAGS      += -ffreestanding -std=gnu11
CFLAGS      += $(optimization) $(warnings)

# do not add a rule for explicitly building each dependency file
# update dependency file <--> update object file
CPPFLAGS    += -MD -MP
CPPFLAGS    += $(addprefix -I ,$(includes))

ifdef OPTIMIZATION
    optimization    := $(OPTIMIZATION)
else
    optimization    := -O2
endif

includes        := include/
warnings        := -Wall -Wextra -Wmissing-prototypes           \
                   -pedantic                                    \
                   -Wredundant-decls -Winline                   \
                   -Wconversion -Wstrict-prototypes             \
                   #-Werror

ifdef HOSTED
  CPPFLAGS += -DHOSTED
endif

sources := $(wildcard *.c *.S)
objects := $(patsubst %.S,%.o,$(sources:%.c=%.o))

%.o: %.c
	@echo -n 'compiling $<...'
	@$(COMPILE.c) $(OUTPUT_OPTION) $<
	@echo 'done'

%.o: %.S
	@echo -n 'assembling $<...'
	@$(COMPILE.S) -o $@ $<
	@echo 'done'

.PHONY: clean
clean:
	@echo -n cleaning...
	@$(RM) *.o *.d *.log *.elf *.img
	@echo done

-include $(objects:%.o=%.d)
