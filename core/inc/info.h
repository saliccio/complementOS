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
	u32_ct base_address_low;
	u32_ct base_address_high;
	u32_ct length_low;
	u32_ct length_high;
	mmap_entry_type type;
	u32_ct extended_attributes;
} __attribute__((packed)) mmap_entry_t;

u32_ct memory_sanitize_info(mmap_entry_t* dest);