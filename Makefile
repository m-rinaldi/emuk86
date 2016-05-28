modules := lib x86 mm drivers

mod2obj   = $(1:%=$1/%.o)
mod2build = $(1:%=build-%)
mod2clean = $(1:%=clean-%)

# $(call add-mod-build-phony,module-name)
define add-mod-build-phony
    $(eval .PHONY: $(call mod2build,$1))
endef

# $(call add-mod-build-target,module-name)
define add-mod-build-target
    $(eval $(call mod2build,$1):; $$(MAKE) -C $1/)
endef

# $(call connect-mod-build,module-name)
# empty recipe
define connect-mod-build
    $(eval $(call mod2obj,$1): $(call mod2build,$1);)
endef

# $(call add-mod-clean-phony,module-name)
define add-mod-clean-phony
    $(eval .PHONY: $(call mod2clean,$1))
endef

# $(call add-mod-clean-target,module-name)
define add-mod-clean-target
    $(eval $(call mod2clean,$1):; $$(MAKE) -C $1/ clean)
endef

# since both operators && and || have the same precedence and associativity is
# left to right, place parenthesis around
check-status := || (echo 'Fail' && exit 1)

target  := kernel.elf
hdd-img := hdd.img

hdd-img-sectors     := 65536
hdd-img-sector-size := 512
hdd-img-size        := $(shell expr $(hdd-img-sectors) '*' \
                            ${hdd-img-sector-size})

# $(call mount-img image,device)
mount-img = sudo losetup --offset 1048576 --sizelimit $(hdd-img-size) $2 $1

.PHONY: default
default: all

include common.mk

.PHONY: all
all: $(target) hdd.img

module-objects := $(foreach mod,$(modules),$(call mod2obj,$(mod)))
# add a phony target for building each module
$(foreach mod,$(modules),$(call add-mod-build-phony,$(mod)))
$(foreach mod,$(modules),$(call add-mod-build-target,$(mod)))
$(foreach mod,$(modules),$(call connect-mod-build,$(mod)))

.PHONY: clean clean-modules
clean: clean-modules
clean-modules: $(foreach mod,$(modules),$(call mod2clean,$(mod)))

# add a phony target for cleaning each module
$(foreach mod,$(modules),$(call add-mod-clean-phony,$(mod)))
$(foreach mod,$(modules),$(call add-mod-clean-target,$(mod)))

linker-script := kernel.ld
# TODO add (header) dependencies automatically
$(target): kmain.o startup.o $(linker-script) $(module-objects)
	@echo -n linking $@...
	@$(LD) $< $(module-objects) -T $(linker-script)
	@echo done

# implicit rule for image creation
dd      :=  dd
dd_if   :=  /dev/zero
dd_of   =   $@
%.img:
	@echo -n generating image '$@'...
	@$(dd) if=$(dd_if) of=$(dd_of) bs=$(DD_BS) count=$(DD_COUNT) 2>/dev/null \
        $(check-status)
	@echo done

hdd.img: loop-dev := $(shell losetup --find)
# each double-colon rule should specify a recipe, otherwise an implicit rule
# will be used if one applies
# Append a semicolon after the prerequisites in order to prevent the target
# from getting the implicit recipe
hdd.img:: $(if $(wildcard hdd.img),,hdd-unformatted.img);
  # double-color rules' recipe are always executed if there is no prerequisite	
  ifeq ($(wildcard hdd.img),)
	@echo -n formatting '$<'...
	@$(call mount-img,$<,$(loop-dev))
	@sudo mkfs.fat -F 16 -n EMUK86 $(loop-dev) >/dev/null
	@sudo losetup --detach $(loop-dev)
	@$(MV) $< $@
	@echo done
  endif

hdd.img: loop-dev := $(shell losetup --find)
hdd.img: dir      =  $(word 1,$|)
hdd.img:: $(target) | mount
	@echo -n updating hdd image '$(hdd-img)'...
	@$(call mount-img,$(hdd-img),$(loop-dev))
	@sudo mount $(loop-dev) $(dir)
	@sudo cp -f $(target) $(dir) || \
        (sudo umount $(dir) && sudo losetup --detach $(loop-dev))
	@sudo umount $(dir)
	@sudo losetup --detach $(loop-dev)
	@touch $@
	@echo done

mount:
	@mkdir $@

hdd-unformatted.img: hdd-unpartitioned.img
	@echo -n partitioning '$<'...
	@printf "n\np\n\n\n\nw" | fdisk $< >/dev/null $(check-status)
	@$(MV) $< $@
	@echo done

# 32MiB
hdd-unpartitioned.img:  DD_BS       :=  $(hdd-img-sector-size)
hdd-unpartitioned.img:  DD_COUNT    :=  $(hdd-img-sectors)
