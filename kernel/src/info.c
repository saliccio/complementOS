#include "info.h"
#include <array.h>

bool compare_mmap_entries(const void* ptr1, const void* ptr2) {
    mmap_entry_t* entry1_ptr = (mmap_entry_t*)ptr1;
    mmap_entry_t* entry2_ptr = (mmap_entry_t*)ptr2;
    return (entry1_ptr->base_address_low + entry1_ptr->length_low - 1) < (entry2_ptr->base_address_low + entry2_ptr->length_low - 1);  // Subtracting 1 is to prevent overflows
}

u32 memory_sanitize_info(mmap_entry_t* dest) {
    u32 length = *(u32*)MEMORY_INFO_NO_OF_ENTRIES_ADDRESS;

	mmap_entry_t* buffer = (mmap_entry_t*) MEMORY_INFO_BASE_ADDRESS;

    sort(buffer, sizeof(mmap_entry_t), length, compare_mmap_entries);

    dest[0] = buffer[0];
    u32 dest_index = 1;
    for (u32 i = 1; i < length; i++) {
        u32 previous_end_address = buffer[i - 1].base_address_low + buffer[i - 1].length_low;
        if (buffer[i].base_address_low < previous_end_address) {
            dest_index--;
            dest[dest_index].length_low += buffer[i].length_low;
        } else {
            dest[dest_index] = buffer[i];
        }
        dest_index++;
    }

    return dest_index;
}