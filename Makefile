CC=gcc
NASM=nasm
NDISASM=ndisasm
GDB=gdb
LD=ld
QEMU=qemu-system-x86_64
LD_SETTINGS=linker.ld
GDB_ADDRESS=localhost:1234
ARCH=x86
CWD:=$(shell pwd)
BIN:=$(shell realpath bin)
IMAGE:=$(BIN)/image
BOOT_ENTRY_BIN:=$(BIN)/$(ARCH)/src_bootEntry.s.o # This binary must come first in the LD's input list.
LINKED_BIN:=$(BIN)/linked.bin
MODULES=arch drivers kernel libc
CINCLUDE:=-I$(CWD)/include

# -g: Include debug information
# -m32: Compile for IA-32
# -ffreestanding: No standard library
# -nostdlib: No stdlib links by default
# -fno-builtin: Avoid compile-time replacements for stdlib functions
# -fno-pie: No position independent code (PIE), needed for IA-32
CFLAGS=-g -m32 -ffreestanding -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-pie -Wall -Werror -Wextra

build_run: all run

# First, clean all possible binaries created before, then create the OS image
all: clean $(IMAGE)

# Run the OS
run:
	$(QEMU) -fda $(IMAGE)

# Create OS image
$(IMAGE): build_modules
	cat $(wildcard $(BIN)/*/*.bin) $(LINKED_BIN) > $@

build_modules:
	mkdir -p $(BIN)
	$(foreach module,$(MODULES),mkdir -p $(BIN)/$(module);)
	$(foreach module,$(MODULES),$(MAKE) -C $(module) CC=$(CC) NASM=$(NASM) ARCH=$(ARCH) BIN=$(BIN)/$(module) CINCLUDE="$(CINCLUDE)" CFLAGS="$(CFLAGS)";)
	$(LD) -T $(LD_SETTINGS) -o $(LINKED_BIN) $(KERNEL_ENTRY_BIN) $(BIN)/*/*.o

clean:
	rm -rf $(BIN)

# Disassemble OS image (for debugging)
image.dis: $(IMAGE)
	$(NDISASM) -b 32 $< > $@
	
# Start QEMU, then GDB to debug
debug: debug_qemu debug_gdb

# Start QEMU held ready waiting for GDB (-S flag)
debug_qemu: $(IMAGE)
	$(QEMU) -S -fda $<

# Start GDB to debug
debug_gdb: $(IMAGE)
	$(GDB) -ex "target remote $(GDB_ADDRESS)" -ex "symbol-file bin/image"