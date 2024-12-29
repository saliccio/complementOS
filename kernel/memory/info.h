#pragma once

#include <types.h>

#define MEMORY_INFO_NO_OF_ENTRIES_ADDRESS 0x8000
#define MEMORY_INFO_BASE_ADDRESS 0x8004

typedef enum {
	USABLE = 1,
	RESERVED = 2,
	ACPI_RECLAIMABLE = 3,
	ACPI_RESERVED = 4,
	BAD = 5
} mmap_entry_type;

typedef struct {
	u32 base_address_low;
	u32 base_address_high;
	u32 length_low;
	u32 length_high;
	mmap_entry_type type;
	u32 extended_attributes;
} __attribute__((packed)) mmap_entry_t;

u32 memory_sanitize_info(mmap_entry_t* dest);