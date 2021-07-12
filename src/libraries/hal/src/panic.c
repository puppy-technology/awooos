#include <ali/event.h>
#include <dmm.h>
#include "panic.h"
#include <stddef.h>

Hal_PanicFn *_hal_panic = NULL;

void hal_panic_init(Hal_PanicFn *panicfn)
{
    _hal_panic = panicfn;
}

__attribute__((constructor))
void hal_panic_register_events(void)
{
    event_watch("register panic function", (void (*)(void*))hal_panic_init);
    event_watch("register panic function", (void (*)(void*))dmm_init);
}
