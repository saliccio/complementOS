BASE_PATH?=.
C_SRCS=$(wildcard $(BASE_PATH)/src/*.c)
C_OBJS=$(patsubst $(BASE_PATH)/src/%.c,$(BIN)/%.c.o,$(C_SRCS))
ASM_SRCS=$(wildcard $(BASE_PATH)/src/*.s)
ASM_OBJS=$(patsubst $(BASE_PATH)/src/%.s,$(BIN)/%.s.o,$(ASM_SRCS))
MANGLED_OUTPUT_NAME=$(shell echo $(subst /,_,$^)).o

# Generic - Assembly code to object file
$(BIN)/%.s.o: $(BASE_PATH)/src/%.s
	$(NASM) -f elf $^ -o $(BIN)/$(MANGLED_OUTPUT_NAME)

# Generic - C code to object file
$(BIN)/%.c.o: $(BASE_PATH)/src/%.c
	$(CC) $(CINCLUDE) -I$(BASE_PATH)/inc -Iinc $(CFLAGS) -c $< -o $(BIN)/$(MANGLED_OUTPUT_NAME)