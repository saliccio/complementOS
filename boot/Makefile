include ../common.mk

$(BIN)/boot.bin: src/bootloader.s
	$(NASM) -Isrc -f bin $< -o $@