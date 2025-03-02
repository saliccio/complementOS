CC=gcc
NASM=nasm
NDISASM=ndisasm
GDB=gdb
LD=ld
QEMU=qemu-system-x86_64
QEMU_IMG=qemu-img
BOCHS=bochs
BX_IMG=bximage
LD_SETTINGS=linker.ld
GDB_ADDRESS=localhost:1234
ARCH=x86
CWD:=$(shell pwd)
BIN:=$(shell realpath bin)
DISK:=$(BIN)/disk.img
DISK_SIZE:=10M
DD:=dd
IMAGE:=$(BIN)/image
LINKED_BIN:=$(BIN)/linked.bin
MODULES=arch drivers core libc
CINCLUDE:=-I$(CWD)/include

# -g: Include debug information
# -m32: Compile for IA-32
# -ffreestanding: No standard library
# -nostdlib: No stdlib links by default
# -fno-builtin: Avoid compile-time replacements for stdlib functions
# -fno-pie: No position independent code (PIE), needed for IA-32
CFLAGS=-g -m64 -ffreestanding -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -Wall

build_run_qemu: all run_qemu
build_run_bochs: all run_bochs

# First, clean all possible binaries created before, then create the OS image
all: clean $(IMAGE)

# Run the OS via QEMU
run_qemu:
	$(QEMU_IMG) create -f raw $(DISK) $(DISK_SIZE)
	$(DD) if=$(IMAGE) of=$(DISK) conv=notrunc
	$(QEMU) -drive format=raw,file=$(DISK),if=ide -boot a -monitor stdio -s

# Run the OS via Bochs
run_bochs:
	$(BX_IMG) -func=create -q -hd=$(DISK_SIZE) -imgmode=flat $(DISK)
	$(DD) if=$(IMAGE) of=$(DISK) conv=notrunc
	$(BOCHS)

# Create OS image
$(IMAGE): build_modules
	cat $(wildcard $(BIN)/*/*.bin) $(LINKED_BIN) > $@

build_modules:
	mkdir -p $(BIN)
	$(foreach module,$(MODULES),mkdir -p $(BIN)/$(module);)
	$(foreach module,$(MODULES),$(MAKE) -C $(module) CC=$(CC) NASM=$(NASM) ARCH=$(ARCH) BIN=$(BIN)/$(module) CINCLUDE="$(CINCLUDE)" CFLAGS="$(CFLAGS)";)
	$(LD) -T $(LD_SETTINGS) -o $(LINKED_BIN) $(BIN)/*/*.o

clean:
	rm -rf $(BIN)

# Disassemble OS image (for debugging)
image.dis: $(IMAGE)
	$(NDISASM) -b 64 $< > $@
	
# Start QEMU, then GDB to debug
debug: debug_qemu debug_gdb

# Start QEMU held ready waiting for GDB (-S flag)
debug_qemu: $(IMAGE)
	$(QEMU) -S -fda $<

# Start GDB to debug
debug_gdb: $(IMAGE)
	$(GDB) -ex "target remote $(GDB_ADDRESS)" -ex "symbol-file bin/image"