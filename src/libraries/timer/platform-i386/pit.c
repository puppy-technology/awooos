#include "pit.h"
#include <ali/event.h>
#include "../../hal/platform-i386/ports.h"

void pit_phase(int hz) {
    int divisor = 1193182 / hz;                 // Calculate divisor
    hal_outb(0x43, 0x36);                       // Set command byte 0x36
    hal_outb(0x40, divisor & 0xFF);             // Set low byte of divisor
    hal_outb(0x40, (uint8_t)(divisor >> 8));    // Set high byte of divisor
}

void pit_init(__attribute__((unused)) void *data) {
    pit_phase(PIT_FREQUENCY);
}

__attribute__((constructor))
void timer_pit_init_register_events(void) {
    event_watch("HAL init", &pit_init);
}
