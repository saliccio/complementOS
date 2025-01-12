BASE_PATH=$(ARCH)

include ../common.mk

C_SRCS=$(wildcard $(BASE_PATH)/src/*.c)
C_OBJS=$(patsubst $(BASE_PATH)/src/%.c,$(BIN)/%.c.o,$(C_SRCS))
ASM_SRCS=$(wildcard $(BASE_PATH)/src/*.s)
ASM_OBJS=$(patsubst $(BASE_PATH)/src/%.s,$(BIN)/%.s.o,$(ASM_SRCS))

BOOT_BIN=$(BIN)/boot.bin
ASM_SRCS:=$(filter-out $(BASE_PATH)/src/bootloader.s,$(ASM_SRCS)) # Remove bootloader.s from main binary (keep it in bootloader binary)

all: $(BOOT_BIN) $(C_OBJS) $(ASM_OBJS)

$(BOOT_BIN): $(BASE_PATH)/src/bootloader.s
	$(NASM) -I$(BASE_PATH)/inc -f bin $< -o $@
