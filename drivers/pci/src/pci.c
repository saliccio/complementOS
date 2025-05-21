#include "drivers/pci.h"
#include "arch/ports.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

u32_ct pci_config_read(u8_ct bus, u8_ct slot, u8_ct func, u8_ct offset)
{
    u32_ct address = (1U << 31) | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC);
    PORT_WRITE_4B(PCI_CONFIG_ADDRESS, address);
    return PORT_READ_4B(PCI_CONFIG_DATA);
}