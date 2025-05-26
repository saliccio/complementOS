#include "ahci.h"
#include "core/addrSpace.h"
#include "core/kString.h"
#include "core/kernelHeap.h"
#include "core/ld.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"
#include "drivers/pci.h"
#include "fis.h"

SECTION(".data") static volatile hba_mem_regs_ct *abar;
SECTION(".data") static volatile hba_port_regs_ct *main_sata_port;

static u32_ct check_port_type(hba_port_regs_ct *port)
{
    u32_ct ssts = port->ssts;

    u8_ct ipm = (ssts >> 8) & 0x0F;
    u8_ct det = ssts & 0x0F;

    if (det != 3 || ipm != 1)
    {
        return AHCI_DEV_NULL; // No device connected
    }

    switch (port->sig)
    {
    case 0x00000101:
        return AHCI_DEV_SATA; // SATA drive
    case 0xEB140101:
        return AHCI_DEV_SATAPI; // ATAPI drive (e.g. DVD)
    case 0xC33C0101:
        return AHCI_DEV_SEMB; // Enclosure management bridge
    case 0x96690101:
        return AHCI_DEV_PM; // Port multiplier
    default:
        return AHCI_DEV_NULL; // Unknown or not present
    }
}

static void start_cmd(hba_port_regs_ct *port)
{
    while (port->cmd & HBA_PxCMD_CR)
    {
        // Wait until CR (command running) is cleared
    }
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}

static void stop_cmd(hba_port_regs_ct *port)
{
    port->cmd &= ~HBA_PxCMD_ST;
    port->cmd &= ~HBA_PxCMD_FRE;
    while (port->cmd & (HBA_PxCMD_FR | HBA_PxCMD_CR))
    {
        // Wait for FR/CR clear
    }
}

static err_code_ct allocate_ahci_port_memory(hba_port_regs_ct *port)
{
    hba_cmd_header_ct *clb = kmalloc_without_header(AHCI_CMD_LIST_SIZE);

    // Allocate Command List Base (CLB) — 1KB aligned
    if (NULL == clb)
    {
        return NO_MEMORY;
    }
    kmemset(clb, 0, AHCI_CMD_LIST_SIZE);

    addr_ct fb = kmalloc_without_header(AHCI_FIS_SIZE);

    // Allocate FIS Receive Buffer (FB) — 256B aligned
    if (NULL == fb)
    {
        return NO_MEMORY;
    }
    kmemset(fb, 0, AHCI_FIS_SIZE);

    // Assign to port
    port->clb = (u32_ct)clb;
    port->clbu = 0;
    port->fb = (u32_ct)fb;
    port->fbu = 0;

    // Allocate command tables (1 per slot)
    addr_ct *cmd_tables[AHCI_MAX_COMMANDS];
    for (u32_ct i = 0; i < AHCI_MAX_COMMANDS; i++)
    {
        cmd_tables[i] = kmalloc_without_header(AHCI_CMD_TABLE_SIZE);
        if (NULL == cmd_tables[i])
        {
            return NO_MEMORY;
        }
        kmemset(cmd_tables[i], 0, AHCI_CMD_TABLE_SIZE);

        clb[i].prdtl = AHCI_PRDT_ENTRIES;
        clb[i].ctba = (u32_ct)(addr_ct)cmd_tables[i];
        clb[i].ctbau = 0;
    }

    return NO_ERROR;
}

static err_code_ct init_ports(hba_mem_regs_ct *abar)
{
    err_code_ct ret = NO_ERROR;
    u32_ct pi = abar->pi;
    for (u32_ct i = 0; i < 32; i++)
    {
        if (pi & (1 << i))
        {
            hba_port_regs_ct *port = &abar->ports[i];
            u32_ct type = check_port_type(port);
            if (AHCI_DEV_SATA == type)
            {
                vga_printf("SATA drive found at port %d\n", i);

                if (NULL == main_sata_port)
                {
                    main_sata_port = port;
                    vga_printf("Port %d will be used for the main SATA disk.\n", i);
                }

                ret = allocate_ahci_port_memory(port);
                if (NO_ERROR != ret)
                {
                    return ret;
                }

                /* Basic SATA init */
                stop_cmd(port);
                start_cmd(port);
            }
            else if (AHCI_DEV_SATAPI == type)
            {
                vga_printf("SATAPI drive found at port %d\n", i);
            }
            else if (AHCI_DEV_SEMB == type)
            {
                vga_printf("SEMB device found at port %d\n", i);
            }
            else if (AHCI_DEV_PM == type)
            {
                vga_printf("Port Multiplier found at port %d\n", i);
            }
        }
    }

    return ret;
}

static u32_ct find_cmd_slot(hba_port_regs_ct *port)
{
    u32_ct slots = (port->sact | port->ci);
    for (u32_ct i = 0; i < 32; i++)
    {
        if ((slots & (1 << i)) == 0)
        {
            return i;
        }
    }
    return -1; // No free slot
}

static inline err_code_ct ahci_rw_inner(hba_port_regs_ct *port, u64_ct start_lba, u32_ct count, addr_ct buffer,
                                        bool_ct is_write)
{
    port->is = (u32_ct)-1; // Clear pending interrupt bits

    u32_ct slot;
    while ((slot = find_cmd_slot(port)) == -1)
    {
        /* Wait until a command slot becomes available */
    }

    hba_cmd_header_ct *cmd_header = (hba_cmd_header_ct *)port->clb;
    cmd_header += slot;
    cmd_header->cfl = sizeof(fis_reg_host2dev_ct) / sizeof(u32_ct); // Command FIS size (in DWORDS)
    cmd_header->w = is_write ? 1 : 0;
    cmd_header->prdtl = 1; // One PRDT entry

    hba_cmd_table_ct *cmd_tbl = (hba_cmd_table_ct *)(size_ct)(cmd_header->ctba);
    kmemset(cmd_tbl, 0, sizeof(hba_cmd_table_ct) + sizeof(hba_prdt_entry_ct));

    // Set PRDT
    cmd_tbl->prdt[0].dba = (u64_ct)(size_ct)buffer;
    cmd_tbl->prdt[0].dbc = (count * 512) - 1; // byte count
    cmd_tbl->prdt[0].i = 0;                   // Interrupt on completion

    // Setup Command FIS
    fis_reg_host2dev_ct *fis = (fis_reg_host2dev_ct *)(&cmd_tbl->cfis);

    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c = 1; // command
    fis->command = is_write ? ATA_CMD_WRITE_DMA_EX : ATA_CMD_READ_DMA_EX;

    // LBA28/48 support
    fis->lba0 = (u8_ct)(start_lba & 0xFF);
    fis->lba1 = (u8_ct)((start_lba >> 8) & 0xFF);
    fis->lba2 = (u8_ct)((start_lba >> 16) & 0xFF);
    fis->device = 1 << 6; // LBA mode

    fis->lba3 = (u8_ct)((start_lba >> 24) & 0xFF);
    fis->lba4 = (u8_ct)((start_lba >> 32) & 0xFF);
    fis->lba5 = (u8_ct)((start_lba >> 40) & 0xFF);

    fis->countl = count & 0xFF;
    fis->counth = (count >> 8) & 0xFF;

    while ((port->tfd & (0x80 | 0x08)))
    {
        /* Wait until port is not busy */
    }

    port->ci = 1 << slot; // Issue command

    // Wait for completion
    while (1)
    {
        if ((port->ci & (1 << slot)) == 0)
        {
            break; // Done
        }
        if (port->is & (1 << 30))
        {
            return BAD_PARAM; // Task file error
        }
    }

    return NO_ERROR; // Success
}

err_code_ct ahci_rw_main_disk(u64_ct start_lba, u32_ct count, addr_ct buffer, bool_ct is_write)
{
    return ahci_rw_inner(main_sata_port, start_lba, count, buffer, is_write);
}

static err_code_ct ahci_init()
{
    err_code_ct ret = INIT_ERROR;

    for (u32_ct bus = 0; bus < 256; bus++)
    {
        for (u32_ct slot = 0; slot < 32; slot++)
        {
            for (u32_ct func = 0; func < 8; func++)
            {
                u32_ct class_code = pci_config_read(bus, slot, func, 0x08);
                u8_ct base_class = (class_code >> 24) & 0xFF;
                u8_ct sub_class = (class_code >> 16) & 0xFF;
                u8_ct prog_if = (class_code >> 8) & 0xFF;

                if (base_class == 0x01 && sub_class == 0x06 && prog_if == 0x01)
                {
                    u32_ct tmp_abar = pci_config_read(bus, slot, func, 0x24); // BAR5
                    abar = (hba_mem_regs_ct *)(tmp_abar & ~0xF);              // Clear bottom bits
                    ret = mem_map_to_phys_addr(mem_get_kernel_mem_info(), KERNELIZED_ADDR(abar), abar, 1,
                                               PTE_READ_WRITE | PTE_UNCACHEABLE);
                    if (NO_ERROR != ret && ALREADY_MAPPED != ret)
                    {
                        return INIT_ERROR;
                    }

                    abar = KERNELIZED_ADDR(abar);
                    vga_printf("AHCI ABAR = %p\n", abar);

                    ret = init_ports(abar);
                    if (NO_ERROR != ret)
                    {
                        return ret;
                    }

                    return ret;
                }
            }
        }
    }

    return INIT_ERROR;
}

ATTACH_STATIC_HOOK(MMU_INIT_END, ahci_init, 99);