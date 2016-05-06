CC := ~/opt/cross/bin/i686-elf-gcc
LD := ~/opt/cross/bin/i686-elf-ld
AS := ~/opt/cross/bin/i686-elf-as

ECHO    := echo
ECHON   := echo -n

CFLAGS      = -c -ffreestanding -std=gnu11 $(optimization) $(warnings)
CPPFLAGS    += $(aDDprefix -I ,$(include-dirs))

include-dirs    := include/
optimization    := -O2
warnings        := -Wall -Wextra -Wmissing-prototypes           \
                   -pedantic                                    \
                   -Wredundant-decls -Winline                   \
                   -Wconversion -Wstrict-prototypes

# since both operators && and || have the same precedence and associativity is
# left to right, place parenthesis around
check-status := || ($(ECHO) 'Fail' && exit 1)

.PHONY: all clean

TARGET := kernel.elf

all: $(TARGET) update-hdd-image

%.o: %.S
	$(AS) -c $< -o $@

# implicit rule for image creation
dd      :=  dd
dd_if   :=  /dev/zero
dd_of   =   $@
%.img:
	@$(ECHON) generating image '$@'...
	@$(dd) if=$(dd_if) of=$(dd_of) bs=$(DD_BS) count=$(DD_COUNT) 2>/dev/null \
        $(check-status)
	@$(ECHO) done

linker-script := kernel.ld

$(linker-script): startup.o;

kernel.elf: kmain.o startup.o $(linker-script)
	$(LD) $< -T $(linker-script)
	#./update_hDD.sh

update-hdd-image: kernel.elf hdd.img

hdd-partitioning: hdd.img
	@$(ECHON) partitioning '$@'...
	@printf "n\np\n\n\n\nw" | fdisk hdd.img >/dev/null $(check-status)
	@$(ECHO) done

# 32MiB
hdd-img-size := 65536
hdd.img:    DD_BS       :=  512
hdd.img:    DD_COUNT    :=  $(hdd-img-size)
	

clean:
	@$(ECHON) cleaning...
	@$(RM) *.o 
	@$(ECHO) done
