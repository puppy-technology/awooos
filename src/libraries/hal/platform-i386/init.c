#include <ali.h>
#include <ali/event.h>
#include <dmm.h>
#include <stddef.h>
#include <stdio.h>

#include "exceptions.h"
#include "idt.h"
#include "multiboot.h"

extern char kernel_comment_start;

void hal_init(void *data)
{
    const char *metadata[] = {
        data,
        "\r\n",
        "Compiled with: ",
        &kernel_comment_start,
        NULL
    };

    puts("  Configuring exception handlers.");
    hal_exceptions_init();
    puts("  Configuring IDT.");
    hal_idt_init();
    puts("  Making the bad assumption that multiboot works.");
    hal_multiboot_init();

    puts("  Initializing Ali (libc).");
    ali_init(&_dmm_malloc, &_dmm_free, &_dmm_realloc);
    puts("  Initializing greeter");
    event_trigger("greeter display", metadata);
    event_trigger("HAL interrupts enable", NULL);
}

__attribute__((constructor))
void hal_init_register_events(void)
{
    event_watch("HAL init", &hal_init);
}
