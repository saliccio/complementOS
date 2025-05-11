#include "arch/isr.h"
#include "drivers/d_screen.h"
#include "idt.h"
#include "pic.h"
#include <arch/ports.h>
#include <libc/conversions.h>

static isr_handler_ct isr_handlers[IDT_NO_OF_ENTRIES];

void isr_set_handler(u8_ct int_no, isr_handler_ct handler)
{
    isr_handlers[int_no] = handler;
}

void exc_general_handler(isr_registers_ct *registers)
{
    PORT_WRITE_BYTE(PIC_MASTER_COMMAND_PORT,
                    PIC_EOI_COMMAND); // ALWAYS send an EOI to the master PIC
    if (registers->int_no >= PIC_SLAVE_STARTING_IRQ_VECTOR)
    { // If the interrupt is handled by the
      // slave PIC, send an EOI to it
        PORT_WRITE_BYTE(PIC_SLAVE_COMMAND_PORT, PIC_EOI_COMMAND);
    }

    isr_handler_ct isr_specific_handler = isr_handlers[registers->int_no];
    bool_ct is_specific_handler_set = isr_specific_handler != 0;
    if (is_specific_handler_set)
    {
        isr_specific_handler(registers);
    }
}

void irq_general_handler(isr_registers_ct *registers)
{
    PORT_WRITE_BYTE(PIC_MASTER_COMMAND_PORT,
                    PIC_EOI_COMMAND); // ALWAYS send an EOI to the master PIC
    if (registers->int_no >= PIC_SLAVE_STARTING_IRQ_VECTOR)
    { // If the interrupt is handled by the
      // slave PIC, send an EOI to it
        PORT_WRITE_BYTE(PIC_SLAVE_COMMAND_PORT, PIC_EOI_COMMAND);
    }

    isr_handler_ct isr_specific_handler = isr_handlers[registers->int_no];
    bool_ct is_specific_handler_set = isr_specific_handler != 0;
    if (is_specific_handler_set)
    {
        isr_specific_handler(registers);
    }
}