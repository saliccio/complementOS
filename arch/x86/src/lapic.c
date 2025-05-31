#include "lapic.h"
#include "arch/asm.h"
#include "core/addrSpace.h"
#include "core/ld.h"
#include "core/sysConfig.h"
#include "decrementer.h"
#include "drivers/d_screen.h"
#include "mmu.h"
#include "pit.h"

#define IPI_INIT 0xC500
#define IPI_DEASSERT 0x8500
#define SIPI_INIT 0x608

#define APIC_BASE_MSR 0x1B
#define APIC_BASE_MSR_ENABLE 0x800
#define APIC_BASE_ADDR_MASK 0xFFFFF000

/* Memory-mapped APIC registers (offsets from APIC base) */
#define APIC_REG_ID 0x20
#define APIC_REG_VERSION 0x30
#define APIC_REG_TPR 0x80
#define APIC_REG_EOI 0xB0
#define APIC_REG_SVR 0xF0
#define APIC_REG_ERR_STATUS 0x280
#define APIC_REG_ICR_LOW 0x300
#define APIC_REG_ICR_HIGH 0x310
#define APIC_REG_LVT_TIMER 0x320
#define APIC_REG_TIMER_INIT 0x380
#define APIC_REG_TIMER_COUNT 0x390
#define APIC_REG_TIMER_DIV 0x3E0

// IPI command definitions
#define APIC_ICR_DELIVERY_FIXED 0x00000000
#define APIC_ICR_DELIVERY_INIT 0x00000500
#define APIC_ICR_DELIVERY_STARTUP 0x00000600
#define APIC_ICR_LEVEL_ASSERT 0x00004000
#define APIC_ICR_TRIGGER_MODE_LEVEL 0x00008000
#define APIC_ICR_DEST_MODE_PHYSICAL 0x00000000
#define APIC_ICR_DEST_MODE_LOGICAL 0x00000800
#define APIC_ICR_DEST_SHORTHAND_NONE 0x00000000
#define APIC_ICR_DEST_SHORTHAND_SELF 0x00040000
#define APIC_ICR_DEST_SHORTHAND_ALL 0x00080000
#define APIC_ICR_DEST_SHORTHAND_OTHER 0x000C0000

#define AP_BOOT_VECTOR 0x8

SECTION(".data") static addr_ct apic_base_addr;
SECTION(".data") static u32_ct no_of_cpus_started;

static inline u64_ct read_msr(u32_ct msr_id)
{
    u32_ct low;
    u32_ct high;
    ASM("rdmsr" : "=a"(low), "=d"(high) : "c"(msr_id));
    return ((u64_ct)high << 32) | low;
}

static inline void write_msr(u32_ct msr_id, u64_ct value)
{
    ASM("wrmsr" : : "c"(msr_id), "a"((u32_ct)value), "d"((u32_ct)(value >> 32)));
}

static inline u32_ct lapic_read(u32_ct reg)
{
    return *((volatile u32_ct *)(apic_base_addr + reg));
}

static inline void lapic_write(u32_ct reg, u32_ct value)
{
    *((volatile u32_ct *)(apic_base_addr + reg)) = value;
}

static void calibrate_timer()
{
    /* Set LAPIC timer divisor to 16 */
    lapic_write(APIC_REG_TIMER_DIV, 0b0101);
    /* Set LAPIC timer interrupt vector and make it one-shot */
    lapic_write(APIC_REG_LVT_TIMER, APIC_TIMER_VECTOR | 0xFF);
    /* Set LAPIC timer initial value (temporary for calibration) */
    lapic_write(APIC_REG_TIMER_INIT, 0xFFFFFFFF);

    pit_wait_ms(10);

    u32_ct elapsed_ticks = 0xFFFFFFFF - lapic_read(APIC_REG_TIMER_COUNT);
    u32_ct lapic_freq_hz = elapsed_ticks * 100;

    vga_printf("LAPIC Timer Frequency for Core %d = %d Hz\n", lapic_get_core_id(), lapic_freq_hz);

    u32_ct lapic_count = lapic_freq_hz / TICK_RATE_HZ;

    /* Set LAPIC timer interrupt vector and make it periodic */
    lapic_write(APIC_REG_LVT_TIMER, APIC_TIMER_VECTOR | (1 << 17));
    /* Set LAPIC timer initial value */
    lapic_write(APIC_REG_TIMER_INIT, lapic_count);
    /* Set LAPIC timer current value */
    lapic_write(APIC_REG_TIMER_COUNT, lapic_count);
}

void lapic_enable()
{
    u64_ct apic_msr = read_msr(APIC_BASE_MSR);
    apic_msr |= APIC_BASE_MSR_ENABLE;
    write_msr(APIC_BASE_MSR, apic_msr);

    /* Set Spurious Interrupt Vector Register to enable APIC */
    lapic_write(APIC_REG_SVR, lapic_read(APIC_REG_SVR) | 0x100);

    calibrate_timer();
}

err_code_ct lapic_init()
{
    err_code_ct ret = NO_ERROR;

    u64_ct apic_msr = read_msr(APIC_BASE_MSR);
    apic_base_addr = (u32_ct)(apic_msr & APIC_BASE_ADDR_MASK);
    vga_printf("APIC Base Address = %p\n", apic_base_addr);

    ret = mem_map_virt_addr(mem_get_kernel_mem_info(), KERNELIZED_ADDR(apic_base_addr), apic_base_addr, 1,
                            PTE_READ_WRITE | PTE_UNCACHEABLE);

    if (NO_ERROR != ret)
    {
        return ret;
    }

    lapic_enable();

    decrementer_init();

    return ret;
}

/**
 * Send startup IPI to a specific CPU
 */
static void send_startup_ipi(u32_ct apic_id, u32_ct vector)
{
    /* Clear APIC errors */
    lapic_write(APIC_REG_ERR_STATUS, 0);

    /* Set destination CPU in ICR high */
    lapic_write(APIC_REG_ICR_HIGH, (lapic_read(APIC_REG_ICR_HIGH) & 0x00ffffff) | (apic_id << 24));

    /* Send STARTUP IPI with the vector (page number of the AP start address) */
    lapic_write(APIC_REG_ICR_LOW, (lapic_read(APIC_REG_ICR_LOW) & 0xfff0f800) | 0x608 | vector);

    pit_wait_ms(1);

    /* Wait for IPI to be delivered */
    while (lapic_read(APIC_REG_ICR_LOW) & 0x1000)
    {
        ASM("pause" : : : "memory");
    }
}

/**
 * Start APs (secondary cores)
 */
void lapic_start_aps(void)
{
    for (u32_ct apic_id = 1; apic_id < CORE_COUNT; apic_id++)
    {
        /* Clear APIC errors */
        lapic_write(APIC_REG_ERR_STATUS, 0);

        /* Set destination CPU */
        lapic_write(APIC_REG_ICR_HIGH, (lapic_read(APIC_REG_ICR_HIGH) & 0x00ffffff) | (apic_id << 24));

        /* Send INIT IPI (assert) */
        lapic_write(APIC_REG_ICR_LOW, (lapic_read(APIC_REG_ICR_LOW) & 0xfff00000) | 0x00C500);

        /* Wait for any pending IPIs */
        while (lapic_read(APIC_REG_ICR_LOW) & 0x1000)
        {
            ASM("pause" : : : "memory");
        }

        /* Set destination CPU */
        lapic_write(APIC_REG_ICR_HIGH, (lapic_read(APIC_REG_ICR_HIGH) & 0x00ffffff) | (apic_id << 24));

        /* Send INIT IPI (deassert) */
        lapic_write(APIC_REG_ICR_LOW, (lapic_read(APIC_REG_ICR_LOW) & 0xfff00000) | 0x008500);

        /* Wait for any pending IPIs */
        while (lapic_read(APIC_REG_ICR_LOW) & 0x1000)
        {
            ASM("pause" : : : "memory");
        }

        pit_wait_ms(10);

        /* Send STARTUP IPI (twice, as recommended) */
        /* Vector is the physical page number of the AP start address */
        for (u32_ct i = 0; i < 2; i++)
        {
            send_startup_ipi(apic_id, AP_BOOT_VECTOR);
        }
    }
}

u32_ct lapic_get_core_id()
{
    u32_ct apic_id_register = lapic_read(APIC_REG_ID);
    return (apic_id_register >> 24) & 0xFF; // Bits [31:24] hold the APIC ID
}

void lapic_eoi()
{
    lapic_write(APIC_REG_EOI, 0);
}