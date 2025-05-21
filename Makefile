CC=gcc
NASM=nasm
NDISASM=ndisasm
GDB=gdb
LD=ld
CORE_COUNT=4
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
LINKED_BIN:=$(BIN)/kernel.elf
MODULES=arch drivers core libc
TEST_MODULE=tests
CINCLUDE:=-I$(CWD)/include
TEST_DIRS=$(CWD)/include $(foreach mod, $(MODULES), $(wildcard $(CWD)/$(mod)/inc $(CWD)/$(mod)/*/inc))
TEST_CINCLUDE=$(patsubst %, -I%, $(TEST_DIRS))

# -g: Include debug information.
# -m64: Compile for 64 bits.
# -ffreestanding: No standard library.
# -nostdlib: No stdlib links by default.
# -fno-builtin: Avoid compile-time replacements for stdlib functions.
# -fno-pie: No position independent code (PIE).
# -mcmodel=large: Place no memory restrictions on code or data. All accesses of code and data must be done with absolute addressing.
CFLAGS=-g -m64 -ffreestanding -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-pic -mcmodel=large -Wall -DCORE_COUNT=$(CORE_COUNT)

QEMU_ARGS:=-drive id=disk,file=$(DISK),if=none -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0 -monitor stdio -smp cores=$(CORE_COUNT) -s

build_run_qemu: all run_qemu
build_debug_qemu: all debug_qemu
build_run_tests_qemu: test run_qemu
build_debug_tests_qemu: test debug_qemu

build_run_bochs: all run_bochs
build_run_tests_bochs: test run_bochs

# Test build mode
test: clean build_modules_in_test_mode $(IMAGE)

# First, clean all possible binaries created before, then create the OS image
all: clean build_modules $(IMAGE)

# Run the OS via QEMU
run_qemu: $(IMAGE)
	$(QEMU) $(QEMU_ARGS)

debug_qemu: $(IMAGE)
	$(QEMU) $(QEMU_ARGS) -S

# Run the OS via Bochs
run_bochs: $(IMAGE)
	$(BX_IMG) -func=create -q -hd=$(DISK_SIZE) -imgmode=flat $(DISK)
	$(DD) if=$(IMAGE) of=$(DISK) conv=notrunc
	$(BOCHS)

# Create OS image
$(IMAGE):
	$(LD) -T $(LD_SETTINGS) -o $(LINKED_BIN) $(BIN)/*/*.o
	cat $(wildcard $(BIN)/*/*.bin) $(LINKED_BIN) > $@
	$(QEMU_IMG) create -f raw $(DISK) $(DISK_SIZE)
	$(DD) if=$(IMAGE) of=$(DISK) conv=notrunc

build_modules:
	mkdir -p $(BIN)
	$(foreach module,$(MODULES),mkdir -p $(BIN)/$(module);)
	$(foreach module,$(MODULES),$(MAKE) -C $(module) CC=$(CC) NASM=$(NASM) ARCH=$(ARCH) CORE_COUNT=$(CORE_COUNT) BIN=$(BIN)/$(module) CINCLUDE="$(CINCLUDE)" CFLAGS="$(CFLAGS)";)

build_modules_in_test_mode: build_modules
	mkdir -p $(BIN)/$(TEST_MODULE);
	$(MAKE) -C $(TEST_MODULE) CC=$(CC) NASM=$(NASM) ARCH=$(ARCH) CORE_COUNT=$(CORE_COUNT) BIN=$(BIN)/$(TEST_MODULE) CINCLUDE="$(TEST_CINCLUDE)" CFLAGS="$(CFLAGS)"

clean:
	rm -rf $(BIN)

# Disassemble OS image (for debugging)
image.dis: $(IMAGE)
	$(NDISASM) -b 64 $< > $@