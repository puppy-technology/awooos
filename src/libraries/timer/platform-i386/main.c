#include <stdio.h>
#include <timer.h>
#include <ali/event.h>
#include "../../hal/platform-i386/irq.h"
#include "pit.h"

void timer_event(void *_data) {
    (void)_data;

    gtimer_increment_us(PIT_INTERVAL);
}

__attribute__((constructor))
void timer_init_register_events(void) {
    event_watch(irq_names[0], &timer_event);
}
