INCLUDE=-Ikernel -Idrivers

C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
C_HEADERS = $(wildcard kernel/*.h drivers/*.h)

OBJ = ${C_SOURCES:.c=.o}
BIN = $(wildcard boot/*.bin boot/*.o kernel/*.bin kernel/*.o drivers/*.bin drivers/*.o)
LINKER_SCRIPT = linker_settings.lds

# First, clean all possible binaries created before, then create the OS image
all: clean bin/image

# Run the OS
run: bin/image
	qemu-system-x86_64 -fda $<

# Create OS image
bin/image: bin/boot.bin bin/kernel.bin
	cat $^ > $@

# Create binary for boot sector
bin/boot.bin: boot/bootloader.asm
	mkdir -p bin
	nasm -i boot -f bin $< -o $@

# Create binary for kernel
bin/kernel.bin: kernel/kernel_entry.o ${OBJ}
	ld -m elf_i386 -T $(LINKER_SCRIPT) -o $@ --oformat binary $^

# Generic - Assembly code to object file
%.o: %.asm
	nasm -f elf $^ -o $@

# Generic - C code to object file (Give kernel folder as include path)
%.o: %.c ${C_HEADERS}
	gcc $(INCLUDE) -m32 -ffreestanding -c $< -o $@ -fno-pie
	
# Generic - Assembly code to binary file
%.bin: %.asm
	nasm -f bin $^ -o $@

clean:
	rm -rf bin *.bin *.o *.dis image
	rm -rf ${BIN}

# Disassemble OS image (for debugging)
image.dis: image
	ndisasm -b 32 $< > $@
	
# Disassemble kernel binary (for debugging)
kernel.dis: kernel.bin
	ndisasm -b 32 $< > $@