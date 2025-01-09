include ../common.mk

$(BIN)/boot.bin: src/bootloader.s
	$(NASM) -Iinc -f bin $< -o $@