#include <ali/event.h>
#include <flail.h>
#include <stdio.h>

void hal_flail_init(void *data)
{
    const char *kernel_info = (char*)data;

    flail_init(kernel_info, &putchar);

    event_trigger("register panic function", (void*)&_flail_panic);
}

__attribute__((constructor))
void hal_flail_register_events(void)
{
    event_watch("HAL init", &hal_flail_init);
}
