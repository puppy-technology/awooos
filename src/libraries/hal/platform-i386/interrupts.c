#include <ali/event.h>
#include <ali/modifiers.h>

void hal_interrupts_enable(UNUSED void *data)
{
    __asm__ volatile ("sti");
}

void hal_interrupts_disable(UNUSED void *data)
{
    __asm__ volatile ("cli");
}

__attribute__((constructor))
void hal_interrupts_register_events(void)
{
    event_watch("HAL interrupts enable",   &hal_interrupts_enable);
    event_watch("HAL interrupts disable",  &hal_interrupts_disable);
}
