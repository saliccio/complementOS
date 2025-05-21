#pragma once

/**
 * Frame Information Structures
 */

#include "types.h"

typedef enum fis_type
{
    FIS_TYPE_REG_H2D = 0x27,   // Register FIS - host to device
    FIS_TYPE_REG_D2H = 0x34,   // Register FIS - device to host
    FIS_TYPE_DMA_ACT = 0x39,   // DMA activate FIS - device to host
    FIS_TYPE_DMA_SETUP = 0x41, // DMA setup FIS - bidirectional
    FIS_TYPE_DATA = 0x46,      // Data FIS - bidirectional
    FIS_TYPE_BIST = 0x58,      // BIST activate FIS - bidirectional
    FIS_TYPE_PIO_SETUP = 0x5F, // PIO setup FIS - device to host
    FIS_TYPE_DEV_BITS = 0xA1,  // Set device bits FIS - device to host
} fis_type_ct;

typedef struct fis_reg_host2dev
{
    // DWORD 0
    u8_ct fis_type; // FIS_TYPE_REG_H2D

    u8_ct pmport : 4;    // Port multiplier
    u8_ct reserved0 : 3; // Reserved
    u8_ct c : 1;         // 1: Command, 0: Control

    u8_ct command;  // Command register
    u8_ct featurel; // Feature register, 7:0

    // DWORD 1
    u8_ct lba0;   // LBA low register, 7:0
    u8_ct lba1;   // LBA mid register, 15:8
    u8_ct lba2;   // LBA high register, 23:16
    u8_ct device; // Device register

    // DWORD 2
    u8_ct lba3;     // LBA register, 31:24
    u8_ct lba4;     // LBA register, 39:32
    u8_ct lba5;     // LBA register, 47:40
    u8_ct featureh; // Feature register, 15:8

    // DWORD 3
    u8_ct countl;  // Count register, 7:0
    u8_ct counth;  // Count register, 15:8
    u8_ct icc;     // Isochronous command completion
    u8_ct control; // Control register

    // DWORD 4
    u8_ct reserved1[4]; // Reserved
} fis_reg_host2dev_ct;

typedef struct fis_reg_dev2host
{
    // DWORD 0
    u8_ct fis_type; // FIS_TYPE_REG_D2H

    u8_ct pmport : 4;    // Port multiplier
    u8_ct reserved0 : 2; // Reserved
    u8_ct i : 1;         // Interrupt bit
    u8_ct reserved1 : 1; // Reserved

    u8_ct status; // Status register
    u8_ct error;  // Error register

    // DWORD 1
    u8_ct lba0;   // LBA low register, 7:0
    u8_ct lba1;   // LBA mid register, 15:8
    u8_ct lba2;   // LBA high register, 23:16
    u8_ct device; // Device register

    // DWORD 2
    u8_ct lba3;      // LBA register, 31:24
    u8_ct lba4;      // LBA register, 39:32
    u8_ct lba5;      // LBA register, 47:40
    u8_ct reserved2; // Reserved

    // DWORD 3
    u8_ct countl;       // Count register, 7:0
    u8_ct counth;       // Count register, 15:8
    u8_ct reserved3[2]; // Reserved

    // DWORD 4
    u8_ct reserved4[4]; // Reserved
} fis_reg_dev2host_ct;

typedef struct fis_data
{
    // DWORD 0
    u8_ct fis_type; // FIS_TYPE_DATA

    u8_ct pmport : 4;    // Port multiplier
    u8_ct reserved0 : 4; // Reserved

    u8_ct reserved1[2]; // Reserved

    // DWORD 1 ~ N
    u32_ct data[1]; // Payload
} fis_data_ct;

typedef struct fis_pio_setup
{
    // DWORD 0
    u8_ct fis_type; // FIS_TYPE_PIO_SETUP

    u8_ct pmport : 4;    // Port multiplier
    u8_ct reserved0 : 1; // Reserved
    u8_ct d : 1;         // Data transfer direction, 1 - device to host
    u8_ct i : 1;         // Interrupt bit
    u8_ct reserved1 : 1;

    u8_ct status; // Status register
    u8_ct error;  // Error register

    // DWORD 1
    u8_ct lba0;   // LBA low register, 7:0
    u8_ct lba1;   // LBA mid register, 15:8
    u8_ct lba2;   // LBA high register, 23:16
    u8_ct device; // Device register

    // DWORD 2
    u8_ct lba3;      // LBA register, 31:24
    u8_ct lba4;      // LBA register, 39:32
    u8_ct lba5;      // LBA register, 47:40
    u8_ct reserved2; // Reserved

    // DWORD 3
    u8_ct countl;    // Count register, 7:0
    u8_ct counth;    // Count register, 15:8
    u8_ct reserved3; // Reserved
    u8_ct e_status;  // New value of status register

    // DWORD 4
    u16_ct tc;          // Transfer count
    u8_ct reserved4[2]; // Reserved
} fis_pio_setup_ct;

typedef struct fis_dma_setup
{
    // DWORD 0
    u8_ct fis_type; // FIS_TYPE_DMA_SETUP

    u8_ct pmport : 4;    // Port multiplier
    u8_ct reserved0 : 1; // Reserved
    u8_ct d : 1;         // Data transfer direction, 1 - device to host
    u8_ct i : 1;         // Interrupt bit
    u8_ct a : 1;         // Auto-activate. Specifies if DMA Activate FIS is needed

    u8_ct reserved1[2]; // Reserved

    // DWORD 1&2

    u64_ct DMAbufferID; // DMA Buffer Identifier. Used to Identify DMA buffer in host memory.
                        // SATA Spec says host specific and not in Spec. Trying AHCI spec might work.

    // DWORD 3
    u32_ct reserved2; // More reserved

    // DWORD 4
    u32_ct DMAbufOffset; // Byte offset into buffer. First 2 bits must be 0

    // DWORD 5
    u32_ct TransferCount; // Number of bytes to transfer. Bit 0 must be 0

    // DWORD 6
    u32_ct reserved3; // Reserved
} fis_dma_setup_ct;

typedef struct fis_dev_bits
{
    // DWORD 0
    u8_ct fis_type; // FIS_TYPE_DEV_BITS

    u8_ct pmport : 4;    // Port multiplier
    u8_ct reserved0 : 2; // Reserved
    u8_ct i : 1;         // Interrupt bit
    u8_ct n : 1;

    u8_ct statusl : 3;
    u8_ct reserved1 : 1;
    u8_ct statush : 3;
    u8_ct reserved2 : 1;

    u8_ct error;
} fis_dev_bits_ct;

typedef volatile struct hba_fis
{
    // 0x00
    fis_dma_setup_ct dsfis; // DMA Setup FIS
    u8_ct pad0[4];

    // 0x20
    fis_pio_setup_ct psfis; // PIO Setup FIS
    u8_ct pad1[12];

    // 0x40
    fis_reg_dev2host_ct rfis; // Register – Device to Host FIS
    u8_ct pad2[4];

    // 0x58
    fis_dev_bits_ct sdbfis; // Set Device Bit FIS

    // 0x60
    u8_ct ufis[64];

    // 0xA0
    u8_ct reserved0[0x100 - 0xA0];
} hba_fis_ct;