#include <ali/event.h>
#include <ali/modifiers.h>
#include <keyboard.h>
#include "irq.h"
#include "keysym_us.h"
#include "ports.h"
#include <stdlib.h>

#define EMPTY_KEYBOARD_EVENT {0,0,0,0,0,0,0,0,0,0,0,0,{0,},0,0}

#define IS_BREAK(x) (scancode & 0x80)
#define IS_MAKE(x)  (!IS_BREAK(x))

#define MAKE(x)  (scancode == x)
#define BREAK(x) (scancode == (x + 0x80))

#define ACTIVE(key, scancode) (MAKE(scancode) || (state.key && (!BREAK(scancode))))

static KeyboardEvent state = EMPTY_KEYBOARD_EVENT;

char hal_keyboard_resolve_scancode(char keysym[128], uint32_t scancode)
{
    return keysym[scancode & 0x7F];
}

void hal_keyboard_callback(UNUSED void *data)
{
    uint32_t scancode = hal_inb(0x60);

    KeyboardEvent event = EMPTY_KEYBOARD_EVENT;

    event.make       = IS_MAKE(scancode);
    event.Esc        = ACTIVE(Esc,   0x01);
    event.CtrlL      = ACTIVE(CtrlL, 0x1D);
    event.CtrlR      = false; //ACTIVE(CtrlR, ); // TODO: Add CtrlR support.
    event.AltL       = ACTIVE(AltL,  0x38);
    event.AltR       = false; //ACTIVE(AltR, );  // TODO: Add AltR support.
    event.GuiL       = false; //ACTIVE(GuiL, );  // TODO: Add GuiL support.
    event.GuiR       = false; //ACTIVE(GuiR, );  // TODO: Add GuiR support.
    event.ShiftL     = ACTIVE(ShiftL,    0x2A);
    event.ShiftR     = ACTIVE(ShiftR,    0x36);
    event.NumLock    = ACTIVE(NumLock,   0x45);
    event.CapsLock   = ACTIVE(CapsLock,  0x3A);
    event.ScrollLock = ACTIVE(ScrollLock, 0x46);

    state.Esc      = event.Esc;
    state.CtrlL    = event.CtrlL;
    state.CtrlR    = event.CtrlR;
    state.AltL     = event.AltL;
    state.AltR     = event.AltR;
    state.GuiL     = event.GuiL;
    state.GuiR     = event.GuiR;
    state.ShiftL   = event.ShiftL;
    state.ShiftR   = event.ShiftR;

    if (event.NumLock && !event.make) {
        state.NumLock = !(state.NumLock);
    }

    if (event.CapsLock && !event.make) {
        state.CapsLock = !(state.CapsLock);
    }

    if (event.ScrollLock && !event.make) {
        state.ScrollLock = !(state.ScrollLock);
    }

    if (((event.ShiftL || event.ShiftR) && !state.CapsLock)
            || (!(event.ShiftL || event.ShiftR) && state.CapsLock)){
        event.c = hal_keyboard_resolve_scancode(keysym_us_shift, scancode);
    } else {
        event.c = hal_keyboard_resolve_scancode(keysym_us, scancode);
    }

    event_trigger("keyboard event", &event);
}

__attribute__((constructor))
void hal_keyboard_register_events(void)
{
    // IRQ events are registered with the names specified in `irq_names`.
    //
    // IRQ 1 = keyboard.
    event_watch(irq_names[1], &hal_keyboard_callback);
}
