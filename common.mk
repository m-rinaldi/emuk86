CC := ~/opt/cross/bin/i686-elf-gcc
LD := ~/opt/cross/bin/i686-elf-ld

MV := mv -f

CFLAGS      += -c -ffreestanding -nostdlib -std=gnu11
CFLAGS      += $(optimization) $(warnings)

CPPFLAGS    += -MD -MP
CPPFLAGS    += $(addprefix -I ,$(includes))

includes        := include/
optimization    := -O2
warnings        := -Wall -Wextra -Wmissing-prototypes           \
                   -pedantic                                    \
                   -Wredundant-decls -Winline                   \
                   -Wconversion -Wstrict-prototypes

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

clean:
	@echo -n cleaning...
	@$(RM) *.o *.d *.img
	@echo done

-include $(objects:%.o=%.d)