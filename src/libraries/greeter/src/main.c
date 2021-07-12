#include <ali/event.h>
#include <stdio.h>

void greeter_display_greeting(void *data)
{
    char **metadata = (char**)data;

    for (size_t idx = 0; metadata[idx] != NULL; idx++) {
        fputs(metadata[idx], stdout);
    }

    fputs("\r\n\r\n", stdout);
}

__attribute__((constructor))
void greeter_register_events(void)
{
    event_watch("greeter display", &greeter_display_greeting);
}
