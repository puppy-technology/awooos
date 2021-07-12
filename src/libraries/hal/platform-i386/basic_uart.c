/* NOTE: THIS UART IMPLEMENTATION IS WRITE-ONLY, TO PRINT DEBUG INFORMATION. */

#include <ali/event.h>
#include "ports.h"
#include <stdint.h>

// http://wiki.osdev.org/Serial_ports
static uint16_t PORT = 0x3F8;
static uint16_t LINE_CONTROL_REGISTER;

static int initialized = 0;

// Initialize the serial port.
void hal_basic_uart_init(void)
{
    LINE_CONTROL_REGISTER = PORT + 5;

    hal_outb(PORT + 1, 0x00);    // Disable all interrupts
    hal_outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    hal_outb(PORT + 0, 0x03);    // Set divisor to 3 (low  byte) 38400 baud
    hal_outb(PORT + 1, 0x00);    //                  (high byte)
    hal_outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    hal_outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    hal_outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

// Returns true if you can transmit, or false otherwise.
int hal_basic_uart_can_send(void)
{
    return hal_inb(LINE_CONTROL_REGISTER) & 0x20;
}

void hal_basic_uart_wait(void)
{
    while(!hal_basic_uart_can_send()) {
        // Do nothing until it's okay to send data.
    }
}

// Print a string via the serial port.
void hal_basic_uart_print(const char *string)
{
    if (!initialized) {
        hal_basic_uart_init();
        initialized = true;
    }

    for (; 0 != *string; string++)
    {
        // Wait until we can write.
        hal_basic_uart_wait();

        // Write the current character.
        hal_outb(PORT, (uint8_t)*string);
    }
}

__attribute__((constructor))
void hal_basic_uart_register_events(void)
{
    event_watch("print string", (void (*)(void*))&hal_basic_uart_print);
}
