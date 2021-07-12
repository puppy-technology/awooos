#include <ali/event.h>
#include <ali/str.h>
#include <ali/number.h>
#include <keyboard.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <timer.h>

// A 1KB buffer for command input should be enough.
// If it's not, please rethink some life choices.
// Or change this magic number. Either works.
#define SHELL_BUFFER_SIZE 1024

#define COMMAND(name) (strcmp(result->pieces[0], name) == 0)

void shell_print_prompt(void)
{
    fputs("# ", stdout);
}

void shell_run(char *buffer)
{
    ShellSplitResult *result = shellsplit(buffer);
    fputs("\n", stdout);
    if (COMMAND("echo")) {
        for (size_t i = 1; i < result->num_pieces; i++) {
            fputs(result->pieces[i], stdout);
            if (i < result->num_pieces) {
                fputs(" ", stdout);
            } else {
                fputs("\n", stdout);
            }
        }
    } else if (COMMAND("uptime")) {
        print_number(gtimer_in_milliseconds());
        puts(" milliseconds\n");
    } else if (COMMAND("awoo")) {
        fputs(result->pieces[0], stdout);

        if(result->num_pieces >= 1) {
            for(int i = 0; i < atoi(result->pieces[1]); ++i) {
                fputs("o", stdout);
            }
        }

        fputs("\n", stdout);
    } else {
        fputs("Error: Unknown command: ", stdout);
        puts(result->pieces[0]);
    }
}

void shell_keyboard_callback(void *data)
{
    // Shell buffer + 1 for trailing NULL byte.
    static char buf[SHELL_BUFFER_SIZE + 1] = {0,};
    static size_t idx = 0;
    static char *buffer = (char*)&buf;

    KeyboardEvent *event = (KeyboardEvent*)data;

    if (event->c == '\0') {
        // Not a printable character.
        return;
    }

    if ((event->c == '\n') && (idx == 0)) {
        // No command to run.
        fputs("\n", stdout);
        shell_print_prompt();
        return;
    }

    if (event->c == '\n') {
        // Try runing a command.
        shell_run(buffer);
        memset(buffer, 0, SHELL_BUFFER_SIZE);
        idx = 0;
        return;
    }

    if (!event->make) {
        // We don't need to care about break events.
        return;
    }

    if (idx >= SHELL_BUFFER_SIZE) {
        // Command too long for buffer.
        // TODO: Show some sort of indication that there is a problem.
        return;
    }

    // TODO: Don't hard-code the value for backspace.
    if (event->c == '\x08') {
        // Delete the last character.
        buffer[idx] = 0;
        if (idx > 0) {
            idx--;
        }

        // HACK: Oh my god this is a horrible way to clear the line.
        fputs("\r", stdout);
        for (int i = 0; i < 79; i++) { fputs(" ", stdout); }
        fputs("\r", stdout);
        shell_print_prompt();
        fputs(buffer, stdout);
    } else {
        // Store the character.
        buffer[idx] = event->c;
        idx++;
        fputs(buffer + idx - 1, stdout);
    }
}

void shell_init(__attribute__((unused)) void *data)
{
    puts("AwooOS Interactive Shell.");
    shell_print_prompt();
}

__attribute__((constructor))
void shell_register_events(void)
{
    event_watch("shell init", &shell_init);
    event_watch("keyboard event", &shell_keyboard_callback);
}
