CC := ~/opt/cross/bin/i686-elf-gcc
LD := ~/opt/cross/bin/i686-elf-ld
AS := ~/opt/cross/bin/i686-elf-as

ECHO    := echo
ECHON   := echo -n
MV      := mv -f

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

TARGET  := kernel.elf
hdd-img := hdd.img

hdd-img-sectors     := 65536
hdd-img-sector-size := 512
hdd-img-size        := $(shell expr $(hdd-img-sectors) '*' \
                            ${hdd-img-sector-size})

# $(call mount-img image,device)
mount-img = sudo losetup --offset 1048576 --sizelimit $(hdd-img-size) $2 $1

.PHONY: all clean

all: $(TARGET) update-hdd-img

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
#$(linker-script): startup.o;

$(TARGET): kmain.o startup.o $(linker-script)
	@echo '*** $@' to update '$?'
	@$(LD) $< -T $(linker-script)

.PHONY: update-hdd-img
update-hdd-img: loop-dev := $(shell losetup --find)
update-hdd-img: dir      =  $(word 1,$|)
update-hdd-img: $(TARGET) $(if $(wildcard ${hdd-img}),,${hdd-img}) | mount
	@$(ECHON) updating hdd image '$(hdd-img)'...
	@$(call mount-img,$(hdd-img),$(loop-dev))
	@sudo mount $(loop-dev) $(dir)
	@sudo cp -f $(TARGET) $(dir) || \
        (sudo umount $(dir) && sudo losetup --detach $(loop-dev))
	@sudo umount $(dir)
	@sudo losetup --detach $(loop-dev)
	@touch $@
	@$(ECHO) done

mount:
	@mkdir $@

hdd.img: loop-dev := $(shell losetup --find)
hdd.img: hdd-unformatted.img
	@$(ECHON) formatting '$<'...
	@$(call mount-img,$<,$(loop-dev))
	@sudo mkfs.fat -F 16 -n EMUK86 $(loop-dev) >/dev/null
	@sudo losetup --detach $(loop-dev)
	@$(MV) $< $@
	@$(ECHO) done

hdd-unformatted.img: hdd-unpartitioned.img
	@$(ECHON) partitioning '$@'...
	@printf "n\np\n\n\n\nw" | fdisk $< >/dev/null $(check-status)
	@$(MV) $< $@
	@$(ECHO) done

# 32MiB
hdd-unpartitioned.img:  DD_BS       :=  $(hdd-img-sector-size)
hdd-unpartitioned.img:  DD_COUNT    :=  $(hdd-img-sectors)
	

clean:
	@$(ECHON) cleaning...
	@$(RM) *.o *.img
	@$(ECHO) done
