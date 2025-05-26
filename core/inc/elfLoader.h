#pragma once

#include "errCodes.h"
#include "types.h"

#define EI_NIDENT 16
#define ELFCLASS32 1
#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define EM_X86_64 62

#define PT_LOAD 1
#define PT_NOTE 4

#define PF_X 1
#define PF_W 2
#define PF_R 4

// ELF64 Header
typedef struct elf64_ehdr
{
    u8_ct e_ident[EI_NIDENT]; // ELF magic number and metadata
    u16_ct e_type;            // Object file type (e.g., executable, shared)
    u16_ct e_machine;         // Target architecture (e.g., x86_64)
    u32_ct e_version;         // ELF version (always 1)
    u64_ct e_entry;           // Entry point virtual address
    u64_ct e_phoff;           // File offset to program header table
    u64_ct e_shoff;           // File offset to section header table
    u32_ct e_flags;           // Target-specific flags
    u16_ct e_ehsize;          // Size of this ELF header (in bytes)
    u16_ct e_phentsize;       // Size of one program header entry
    u16_ct e_phnum;           // Number of entries in program header table
    u16_ct e_shentsize;       // Size of one section header entry
    u16_ct e_shnum;           // Number of entries in section header table
    u16_ct e_shstrndx;        // Section header string table index
} elf64_ehdr_ct;

// ELF64 Program Header
typedef struct elf64_phdr
{
    u32_ct p_type;   // Segment type (e.g., PT_LOAD)
    u32_ct p_flags;  // Segment flags (rwx)
    u64_ct p_offset; // Offset in file
    u64_ct p_vaddr;  // Virtual address in memory
    u64_ct p_paddr;  // Physical address (used for loaders)
    u64_ct p_filesz; // Size of segment in file
    u64_ct p_memsz;  // Size of segment in memory
    u64_ct p_align;  // Alignment in memory and file
} elf64_phdr_ct;

size_ct elf_get_load_size(addr_ct elf_start);

err_code_ct elf_load(addr_ct elf_start, bool_ct is_kernel);