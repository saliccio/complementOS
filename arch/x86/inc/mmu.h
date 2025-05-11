#pragma once

#include "arch/mmu.h"

#define EARLY_PML4T_ADDRESS (0x1000)
#define EARLY_PDPT_ADDRESS (0x2000)
#define EARLY_PAGE_DIRECTORY_ADDRESS (0x3000)
#define EARLY_PAGE_TABLE_1_ADDRESS (0x4000)
#define EARLY_PAGE_TABLE_2_ADDRESS (0x5000)

#define PAGE_TABLE_NO_OF_ENTRIES 512

#define VIRTUAL_ADDRESS_OF_PAGE_TABLE(index) (0xFFC00000 + index * 0x1000)

// Extract PML4 index (bits 39-47)
#define PML4_INDEX(addr) (((u64_ct)(addr) >> 39) & 0x1FF)

// Extract page directory pointer table index (bits 30-38)
#define PDPT_INDEX(addr) (((u64_ct)(addr) >> 30) & 0x1FF)

// Extract page directory index (bits 21-29)
#define PD_INDEX(addr) (((u64_ct)(addr) >> 21) & 0x1FF)

// Extract page table index (bits 12-20)
#define PT_INDEX(addr) (((u64_ct)(addr) >> 12) & 0x1FF)

// 2 MB is allocated for page table data structures
#define MEM_AREA_SIZE (1024 * 1024 * 2)

#define PAGE_SHIFT (12)

// PML4 Entry (PML4E)
typedef struct pml4_entry
{
    u64_ct present : 1;         // Must be 1 for valid entries
    u64_ct rw : 1;              // Read/write permission
    u64_ct user_supervisor : 1; // User(1) / Supervisor(0)
    u64_ct pwt : 1;             // Page-level write-through
    u64_ct pcd : 1;             // Page-level cache disable
    u64_ct accessed : 1;        // Set by CPU on access
    u64_ct ignored : 1;         // Ignored
    u64_ct reserved : 1;        // Reserved (must be 0)
    u64_ct ignored2 : 3;        // Ignored
    u64_ct pdpt_address : 40;   // Address of PDPT (physical address, 4KiB aligned)
    u64_ct reserved2 : 11;      // Reserved, must be 0
    u64_ct xd : 1;              // Execute disable (1 = non-executable)
} __attribute__((packed)) pml4_entry_ct;

// Page Directory Pointer Table Entry (PDPTE) that references a page directory
typedef struct pdpt_entry
{
    u64_ct present : 1;
    u64_ct rw : 1;
    u64_ct user_supervisor : 1;
    u64_ct pwt : 1;
    u64_ct pcd : 1;
    u64_ct accessed : 1;
    u64_ct dirty : 1;     // Only for 1GiB pages
    u64_ct page_size : 1; // 1 = 1GiB page, 0 = points to PD
    u64_ct global : 1;    // Global TLB entry, only for 1GiB pages
    u64_ct ignored : 2;
    u64_ct restart : 1; // For ordinary paging, ignored; for HLAT paging, restart (if 1, linear-address translation is
                        // restarted with ordinary paging)
    u64_ct pd_address : 40; // Address of PD (physical address, 4KiB aligned)
    u64_ct reserved2 : 11;  // Reserved, must be 0
    u64_ct xd : 1;
} __attribute__((packed)) pdpt_entry_ct;

// Page Directory Entry (PDE) that references a page table
typedef struct pd_entry
{
    u64_ct present : 1;
    u64_ct rw : 1;
    u64_ct user_supervisor : 1;
    u64_ct pwt : 1;
    u64_ct pcd : 1;
    u64_ct accessed : 1;
    u64_ct dirty : 1;     // Only for 2MiB pages
    u64_ct page_size : 1; // 1 = 2MiB page, 0 = points to PT
    u64_ct global : 1;
    u64_ct ignored : 2;
    u64_ct restart : 1;
    u64_ct pt_address : 40;
    u64_ct reserved2 : 11;
    u64_ct xd : 1;
} __attribute__((packed)) pd_entry_ct;

// Page Table Entry (PTE)
typedef struct pt_entry
{
    u64_ct present : 1;
    u64_ct rw : 1;
    u64_ct user_supervisor : 1;
    u64_ct pwt : 1;
    u64_ct pcd : 1;
    u64_ct accessed : 1;
    u64_ct dirty : 1;
    u64_ct pat : 1; // Page Attribute Table
    u64_ct global : 1;
    u64_ct ignored : 2;
    u64_ct restart : 1;
    u64_ct phy_address : 40;
    u64_ct reserved2 : 8;
    u64_ct protection_key : 3; // if CR4.PKE = 1 or CR4.PKS = 1, this may control the page’s access rights (see
                               // Section 5.6.2); otherwise, it is ignored and not used to control access rights.
    u64_ct xd : 1;
} __attribute__((packed)) pt_entry_ct;

// Common entry type for all table types:
typedef u64_ct table_entry_ct;

struct page_map_info
{
    pml4_entry_ct *pml4;
};