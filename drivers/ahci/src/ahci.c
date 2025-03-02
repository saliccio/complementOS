#include "ahci.h"

#define AHCI_BASE 0x400000 // 4M

static volatile hba_mem_regs_ct *ahci_base = (volatile hba_mem_regs_ct *)0xFEBF0000; // AHCI base memory-mapped address

// Read a sector using AHCI
u32_ct ahci_read(hba_port_regs_ct *port, u64_ct lba, u16_ct count, addr_ct buffer)
{
    port->cmd &= ~0x1; // Stop the command engine
    while (port->cmd & (1 << 15))
        ; // Wait for CR bit to clear

    volatile hba_cmd_header_ct *cmd_header = (hba_cmd_header_ct *)(port->clb);
    cmd_header->cfl = 5;   // 5 DWORDS (20 bytes)
    cmd_header->w = 0;     // Read
    cmd_header->prdtl = 1; // 1 PRDT entry

    volatile hba_cmd_table_ct *cmd_table = (hba_cmd_table_ct *)(cmd_header->ctba);
    cmd_table->prdt[0] = (u64_ct)buffer; // Buffer Address
    cmd_table->prdt[0] |= (count << 16); // Byte Count

    u8_ct *cfis = cmd_table->cfis;
    cfis[0] = 0x27; // Host to Device FIS
    cfis[1] = 0x80; // Command
    cfis[2] = 0x25; // READ DMA EXT
    cfis[4] = (u8_ct)(lba & 0xFF);
    cfis[5] = (u8_ct)((lba >> 8) & 0xFF);
    cfis[6] = (u8_ct)((lba >> 16) & 0xFF);
    cfis[7] = 0x40; // LBA Mode
    cfis[8] = (u8_ct)((lba >> 24) & 0xFF);
    cfis[10] = (u8_ct)((lba >> 32) & 0xFF);
    cfis[12] = (u8_ct)count;

    port->ci = 1; // Issue command
    while (port->ci & 1)
        ; // Wait until command completes

    return 0;
}

// Initialize AHCI and Read Kernel
void ahci_load_kernel()
{
    hba_port_regs_ct *sata_port = ahci_find_sata_drive();
    if (!sata_port)
        return;

    addr_ct kernel_buffer = (addr_ct)0x100000; // Load kernel at 1MB
    ahci_read(sata_port, 1, 8, kernel_buffer); // Read first 8 sectors

    void (*kernel_entry)() = (void (*)())kernel_buffer;
    kernel_entry(); // Jump to kernel
}

// Check device type
static u32_ct check_type(hba_port_regs_ct *port)
{
    u32_ct ssts = port->ssts;

    u8_ct ipm = (ssts >> 8) & 0x0F;
    u8_ct det = ssts & 0x0F;

    if (det != HBA_PORT_DET_PRESENT) // Check drive status
    {
        return AHCI_DEV_NULL;
    }

    if (ipm != HBA_PORT_IPM_ACTIVE)
    {
        return AHCI_DEV_NULL;
    }

    switch (port->sig)
    {
    case SATA_SIG_ATAPI:
        return AHCI_DEV_SATAPI;
    case SATA_SIG_SEMB:
        return AHCI_DEV_SEMB;
    case SATA_SIG_PM:
        return AHCI_DEV_PM;
    default:
        return AHCI_DEV_SATA;
    }
}

void probe_ports(hba_mem_regs_ct *abar)
{
    // Search disk in implemented ports
    u32_ct pi = abar->pi;
    for (u32_ct i = 0; i < NUMBER_OF_PORTS; i++)
    {
        if (pi & 1)
        {
            u32_ct dt = check_type(&abar->ports[i]);
            if (dt == AHCI_DEV_SATA)
            {
                trace_ahci("SATA drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_SATAPI)
            {
                trace_ahci("SATAPI drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_SEMB)
            {
                trace_ahci("SEMB drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_PM)
            {
                trace_ahci("PM drive found at port %d\n", i);
            }
            else
            {
                trace_ahci("No drive found at port %d\n", i);
            }
        }

        pi >>= 1;
        i++;
    }
}

// Start command engine
static void start_cmd(hba_port_regs_ct *port)
{
    // Wait until CR (bit 15) is cleared
    while (port->cmd & HBA_PxCMD_CR)
    {
    }

    // Set FRE (bit 4) and ST (bit 0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}

// Stop command engine
static void stop_cmd(hba_port_regs_ct *port)
{
    // Clear ST (bit 0)
    port->cmd &= ~HBA_PxCMD_ST;

    // Clear FRE (bit 4)
    port->cmd &= ~HBA_PxCMD_FRE;

    // Wait until FR (bit 14), CR (bit 15) are cleared
    while ((port->cmd & HBA_PxCMD_FR) || (port->cmd & HBA_PxCMD_CR))
    {
    }
}

static void port_init(hba_port_regs_ct *port, u32_ct portno)
{
    stop_cmd(port);

    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port
    port->clb = AHCI_BASE + (portno << 10);
    port->clbu = 0;
    memset((addr_ct)(port->clb), 0, 1024);

    // FIS offset: 32K+256*portno
    // FIS entry size = 256 bytes per port
    port->fb = AHCI_BASE + (32 << 10) + (portno << 8);
    port->fbu = 0;
    memset((addr_ct)(port->fb), 0, 256);

    // Command table offset: 40K + 8K*portno
    // Command table size = 256*32 = 8K per port
    hba_cmd_header_ct *cmdheader = (hba_cmd_header_ct *)(port->clb);
    for (u32_ct i = 0; i < NUMBER_OF_PORTS; i++)
    {
        cmdheader[i].prdtl = 8; // 8 prdt entries per command table
                                // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba = AHCI_BASE + (40 << 10) + (portno << 13) + (i << 8);
        cmdheader[i].ctbau = 0;
        memset((addr_ct)cmdheader[i].ctba, 0, 256);
    }

    start_cmd(port); // Start command engine
}