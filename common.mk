C_SRCS=$(wildcard src/*.c)
C_OBJS=$(patsubst src/%.c,$(BIN)/%.c.o,$(C_SRCS))
ASM_SRCS=$(wildcard src/*.s)
ASM_OBJS=$(patsubst src/%.s,$(BIN)/%.s.o,$(ASM_SRCS))
MANGLED_OUTPUT_NAME=$(shell echo $(subst /,_,$^)).o

# Generic - Assembly code to object file
$(BIN)/%.s.o: src/%.s
	$(NASM) -f elf $^ -o $(BIN)/$(MANGLED_OUTPUT_NAME)

# Generic - C code to object file
$(BIN)/%.c.o: src/%.c
	$(CC) $(CINCLUDE) $(CFLAGS) -c $< -o $(BIN)/$(MANGLED_OUTPUT_NAME)