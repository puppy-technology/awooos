#include "exceptions.h"
#include "idt.h"
#include "irq.h"
#include "../src/panic.h"
#include "ports.h"
#include <ali/event.h>

static const char *exceptions[32] = {
    "0 #DE Divide Error",
    "1 #DB RESERVED",
    "2 - NMI Interrupt",
    "3 #BP Breakpoint",
    "4 #OF Overflow",
    "5 #BR BOUND Range Exceeded",
    "6 #UD Invalid Opcode (Undefined Opcode)",
    "7 #NM Device Not Available (No Math Coprocessor)",
    "8 #DF Double Fault",
    "9  Coprocessor Segment Overrun (reserved)",
    "10 #TS Invalid TSS",
    "11 #NP Segment Not Present",
    "12 #SS Stack-Segment Fault",
    "13 #GP General Protection",
    "14 #PF Page Fault",
    "15 - Intel reserved. Do not use.",
    "16 #MF x87 FPU Floating-Point Error (Math Fault)",
    "17 #AC Alignment Check",
    "18 #MC Machine Check",
    "19 #XM SIMD Floating-Point Exception",
    "20 - Intel reserved. Do not use.",
    "21 - Intel reserved. Do not use.",
    "22 - Intel reserved. Do not use.",
    "23 - Intel reserved. Do not use.",
    "24 - Intel reserved. Do not use.",
    "25 - Intel reserved. Do not use.",
    "26 - Intel reserved. Do not use.",
    "27 - Intel reserved. Do not use.",
    "28 - Intel reserved. Do not use.",
    "29 - Intel reserved. Do not use.",
    "30 - Intel reserved. Do not use.",
    "31 - Intel reserved. Do not use."
};

void hal_exception_handler(Registers *r)
{
    if(r->int_no < 32){
        hal_panic((char*)exceptions[r->int_no]);
        return;
    }

    event_trigger(irq_names[r->int_no - 32], r);

    // If we get here, we have an IRQ (interrupts 32 and higher), so
    // we need to send EOIs to the interrupt controllers.

    // For IRQs 9 and up, send an EOI to the secondary controller.
    if (r->int_no > (31 + 8)) {
        hal_outb(0xA0, 0x20);
    }

    // Send an EOI to the primary controller.
    hal_outb(0x20, 0x20);
}

void hal_irq_remap(void)
{
    // Start the initialization sequence.
    hal_outb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);
    hal_outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);

    // Define the PIC vectors.
    hal_outb(PIC1_DATA, 0x20);
    hal_outb(PIC2_DATA, 0x28);

    // Continue initialization sequence.
    hal_outb(PIC1_DATA, 4);
    hal_outb(PIC2_DATA, 2);

    hal_outb(PIC1_DATA, ICW4_8086);
    hal_outb(PIC2_DATA, ICW4_8086);

    // Unmask all interrupts on PIC1.
    hal_outb(PIC1_DATA, 0x0);

    // Unmask all interrupts on PIC2.
    hal_outb(PIC2_DATA, 0x0);
}

#define EXCEPTION(n) extern void hal_exception_##n(void); \
    hal_idt_register_exception(n, (size_t)hal_exception_##n)

void hal_exceptions_init()
{
    hal_irq_remap();

    EXCEPTION(0);
    EXCEPTION(1);
    EXCEPTION(2);
    EXCEPTION(3);
    EXCEPTION(4);
    EXCEPTION(5);
    EXCEPTION(6);
    EXCEPTION(7);
    EXCEPTION(8);
    EXCEPTION(9);
    EXCEPTION(10);
    EXCEPTION(11);
    EXCEPTION(12);
    EXCEPTION(13);
    EXCEPTION(14);
    EXCEPTION(15);
    EXCEPTION(16);
    EXCEPTION(17);
    EXCEPTION(18);
    EXCEPTION(19);
    EXCEPTION(20);
    EXCEPTION(21);
    EXCEPTION(22);
    EXCEPTION(23);
    EXCEPTION(24);
    EXCEPTION(25);
    EXCEPTION(26);
    EXCEPTION(27);
    EXCEPTION(28);
    EXCEPTION(29);
    EXCEPTION(30);
    EXCEPTION(31);
    EXCEPTION(32);
    EXCEPTION(33);
    EXCEPTION(34);
    EXCEPTION(35);
    EXCEPTION(36);
    EXCEPTION(37);
    EXCEPTION(38);
    EXCEPTION(39);
    EXCEPTION(40);
    EXCEPTION(41);
    EXCEPTION(42);
    EXCEPTION(43);
    EXCEPTION(44);
    EXCEPTION(45);
    EXCEPTION(46);
    EXCEPTION(47);
}

#undef EXCEPTION
