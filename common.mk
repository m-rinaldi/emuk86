CC := ~/opt/cross/bin/i686-elf-gcc

is-tty := $(shell stat --format %F -L /proc/$$PPID/fd/1 | \
            grep 'character special file')

ifdef is-tty
    echo-ok := echo -e '\033[0;32mok\033[0m'
    # TODO echo-fail
else
    echo-ok := echo ok
    # TODO echo-fail
endif

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
	@echo -n '$(indentation)compiling  $<...'
	@$(COMPILE.c) $(OUTPUT_OPTION) $<
	@$(echo-ok)

%.o: %.S
	@echo -n '$(indentation)assembling $<...'
	@$(COMPILE.S) -o $@ $<
	@$(echo-ok)

.PHONY: clean
clean:
	@echo -n '$(indentation)cleaning...'
	@$(RM) *.o *.d *.log *.elf *.img
	@$(echo-ok)

-include $(objects:%.o=%.d)
