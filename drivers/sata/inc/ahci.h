#pragma once

#include "types.h"

#define NUMBER_OF_PORTS 32

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SATAPI 2
#define AHCI_DEV_SEMB 3
#define AHCI_DEV_PM 4

#define AHCI_CMD_LIST_SIZE 1024
#define AHCI_FIS_SIZE 256
#define AHCI_CMD_TABLE_SIZE 4096 // Per command slot
#define AHCI_PRDT_ENTRIES 8
#define AHCI_MAX_PORTS 32
#define AHCI_MAX_COMMANDS 32

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_FR 0x4000
#define HBA_PxCMD_CR 0x8000

#define PCI_CLASS_MASS_STORAGE 0x01
#define PCI_SUBCLASS_SATA 0x06
#define PCI_INTERFACE_AHCI 0x01

#define HBA_PORT_DET_PRESENT 0x3
#define HBA_PORT_IPM_ACTIVE 0x1
#define HBA_PxCMD_ST (1 << 0)
#define HBA_PxCMD_FRE (1 << 4)
#define HBA_PxCMD_FR (1 << 14)
#define HBA_PxCMD_CR (1 << 15)

#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_SEMB 0xC33C0101
#define SATA_SIG_PM 0x96690101
#define SATA_SIG_ATA 0x00000101

#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35

// AHCI Port Registers
typedef struct
{
    u32_ct clb;       // Command List Base Address
    u32_ct clbu;      // Command List Base Address (Upper 32 bits)
    u32_ct fb;        // FIS Base Address
    u32_ct fbu;       // FIS Base Address (Upper 32 bits)
    u32_ct is;        // Interrupt Status
    u32_ct ie;        // Interrupt Enable
    u32_ct cmd;       // Command and Status
    u32_ct reserved0; // Reserved
    u32_ct tfd;       // Task File Data (Status)
    u32_ct sig;       // Signature
    u32_ct ssts;      // SATA Status
    u32_ct sctl;      // SATA Control
    u32_ct serr;      // SATA Error
    u32_ct sact;      // SATA Active
    u32_ct ci;        // Command Issue
    u32_ct sntf;      // SATA Notification
    u32_ct fbs;       // FIS-based Switching Control
    u32_ct rsv1[11];  // Reserved
    u32_ct vendor[4]; // Vendor-specific
} __attribute__((packed)) hba_port_regs_ct;

// AHCI Base Memory Registers
typedef struct
{
    u32_ct cap;                              // HBA Capabilities
    u32_ct ghc;                              // Global HBA Control
    u32_ct is;                               // Interrupt Status
    u32_ct pi;                               // Ports Implemented Bitmask
    u32_ct vs;                               // AHCI Version
    u32_ct ccc_ctl;                          // Command Completion Coalescing Control
    u32_ct ccc_pts;                          // Command Completion Coalescing Ports
    u32_ct em_loc;                           // Enclosure Management Location
    u32_ct em_ctl;                           // Enclosure Management Control
    u32_ct cap2;                             // Extended Capabilities
    u32_ct bohc;                             // BIOS/OS Handoff Control and Status
    u8_ct reserved0[0xA0 - 0x2C];            // Reserved
    u8_ct vendor[0x100 - 0xA0];              // Vendor-specific registers
    hba_port_regs_ct ports[NUMBER_OF_PORTS]; // Port control registers
} __attribute__((packed)) hba_mem_regs_ct;

// Command Header
typedef struct
{
    u8_ct cfl : 5; // Command FIS Length (in DWORDs)
    u8_ct a : 1;   // ATAPI
    u8_ct w : 1;   // Write
    u8_ct p : 1;   // Prefetchable
    u8_ct r : 1;   // Reset
    u8_ct b : 1;   // BIST
    u8_ct c : 1;   // Clear Busy
    u8_ct reserved0 : 1;
    u8_ct pmp : 4; // Port Multiplier Port
    u16_ct prdtl;  // PRDT Length
    u32_ct prdbc;  // PRD Byte Count
    u32_ct ctba;   // Command Table Base Address
    u32_ct ctbau;  // Command Table Base Address Upper 32-bits
    u32_ct reserved1[4];
} __attribute__((packed)) hba_cmd_header_ct;

typedef struct
{
    u32_ct dba;       // Data base address
    u32_ct dbau;      // Data base address upper 32 bits
    u32_ct reserved0; // Reserved

    // DWORD 3
    u32_ct dbc : 22;      // Byte count, 4M max
    u32_ct reserved1 : 9; // Reserved
    u32_ct i : 1;         // Interrupt on completion
} hba_prdt_entry_ct;

// Command Table
typedef struct
{
    u8_ct cfis[64];            // Command FIS
    u8_ct acmd[16];            // ATAPI Command, 12 or 16 bytes
    u8_ct reserved0[48];       // Reserved
    hba_prdt_entry_ct prdt[1]; // Physical Region Descriptor Table, 0 ~ 65535
} __attribute__((packed)) hba_cmd_table_ct;
