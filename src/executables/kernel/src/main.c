#include <awoo.h>
#include <ali/event.h>
#include <stdnoreturn.h>

#include "../../../libraries/flail/src/uint_to_str.h"

typedef void (constructor)(void);

extern size_t __CTOR_LIST__;

void hal_basic_uart_print(const char *string);
void hal_basic_display_print(const char *string);


void xprint(char *str) {
    hal_basic_uart_print(str);
    hal_basic_display_print(str);
}
void xprintn(size_t number, size_t radix) {
    static char buffer[UINT64_BUFSIZE] = {0};

    char *tmp = flail_uint_to_str(buffer, number, radix);
    hal_basic_uart_print(tmp);
    hal_basic_display_print(tmp);
}

noreturn void kernel_main(void)
{
    bool test_build = (AWOO_TEST_BUILD == 1);


    // Run all of the constructors.
    xprint("Running initializers:\n");
    constructor **ctors = (constructor**)&__CTOR_LIST__;
    for (size_t i = 0; ctors[i] != NULL; i++) {
        xprint("  ");
        xprintn(i, 10);
        xprint(" @ 0x");
        xprintn((size_t)ctors[i], 16);
        xprint("\n");

        ctors[i]();
    }

    xprint("Done!\n");
    xprint("Initializing HAL...\n");
    event_trigger("HAL init", (void*)AWOO_INFO);
    xprint("Running test suite...\n");
    event_trigger("tests run", &test_build);
    xprint("Starting interactive shell...\n");
    event_trigger("shell init", NULL);

    while(1){
        // Do nothing forever.
    }
}
