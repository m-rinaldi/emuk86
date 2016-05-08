CC := ~/opt/cross/bin/i686-elf-gcc
LD := ~/opt/cross/bin/i686-elf-ld
AS := ~/opt/cross/bin/i686-elf-as

IECHO   := echo -n
FECHO   := echo
MV      := mv -f

CFLAGS      = -c -ffreestanding -std=gnu11 $(optimization) $(warnings)
CPPFLAGS    += $(addprefix -I ,$(include-dirs))

include-dirs    := include/
optimization    := -O2
warnings        := -Wall -Wextra -Wmissing-prototypes           \
                   -pedantic                                    \
                   -Wredundant-decls -Winline                   \
                   -Wconversion -Wstrict-prototypes

# since both operators && and || have the same precedence and associativity is
# left to right, place parenthesis around
check-status := || ($(FECHO) 'Fail' && exit 1)

TARGET  := kernel.elf
hdd-img := hdd.img

hdd-img-sectors     := 65536
hdd-img-sector-size := 512
hdd-img-size        := $(shell expr $(hdd-img-sectors) '*' \
                            ${hdd-img-sector-size})

# $(call mount-img image,device)
mount-img = sudo losetup --offset 1048576 --sizelimit $(hdd-img-size) $2 $1

.PHONY: all clean

all: $(TARGET) hdd.img

%.o: %.S
	$(AS) -c $< -o $@

# implicit rule for image creation
dd      :=  dd
dd_if   :=  /dev/zero
dd_of   =   $@
%.img:
	@$(IECHO) generating image '$@'...
	@$(dd) if=$(dd_if) of=$(dd_of) bs=$(DD_BS) count=$(DD_COUNT) 2>/dev/null \
        $(check-status)
	@$(FECHO) done

linker-script := kernel.ld
# TODO add (header) dependencies automatically
$(TARGET): kmain.o startup.o $(linker-script)
	@$(LD) $< -T $(linker-script)

hdd.img: loop-dev := $(shell losetup --find)
# each double-colon rule should specify a recipe, otherwise an implicit rule
# will be used if one applies
# Append a semicolon after the prerequisites in order to prevent the target
# from getting the implicit recipe
hdd.img:: $(if $(wildcard hdd.img),,hdd-unformatted.img);
  # double-color rules' recipe are always executed if there is no prerequisite	
  ifeq ($(wildcard hdd.img),)
	@$(IECHO) formatting '$<'...
	@$(call mount-img,$<,$(loop-dev))
	@sudo mkfs.fat -F 16 -n EMUK86 $(loop-dev) >/dev/null
	@sudo losetup --detach $(loop-dev)
	@$(MV) $< $@
	@$(FECHO) done
  endif

hdd.img: loop-dev := $(shell losetup --find)
hdd.img: dir      =  $(word 1,$|)
hdd.img:: $(TARGET) | mount
	@$(IECHO) updating hdd image '$(hdd-img)'...
	@$(call mount-img,$(hdd-img),$(loop-dev))
	@sudo mount $(loop-dev) $(dir)
	@sudo cp -f $(TARGET) $(dir) || \
        (sudo umount $(dir) && sudo losetup --detach $(loop-dev))
	@sudo umount $(dir)
	@sudo losetup --detach $(loop-dev)
	@touch $@
	@$(FECHO) done

mount:
	@mkdir $@

hdd-unformatted.img: hdd-unpartitioned.img
	@$(IECHO) partitioning '$<'...
	@printf "n\np\n\n\n\nw" | fdisk $< >/dev/null $(check-status)
	@$(MV) $< $@
	@$(FECHO) done

# 32MiB
hdd-unpartitioned.img:  DD_BS       :=  $(hdd-img-sector-size)
hdd-unpartitioned.img:  DD_COUNT    :=  $(hdd-img-sectors)
	
clean:
	@$(IECHO) cleaning...
	@$(RM) *.o *.img
	@$(FECHO) done
