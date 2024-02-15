#include <avr/io.h>
#include <util/delay.h>
#include "usb_keyboard.h"

static void send_shrug()
{
    usb_keyboard_press(KEY_SPACE, KEY_GUI);

    _delay_ms(500); // wait for spotlight to open
    usb_keyboard_press(KEY_BACKSPACE, 0);

    uint8_t keys[][2] = {
        {KEY_H, 0},
        {KEY_O, 0},
        {KEY_W, 0},

        {KEY_SPACE, 0},

        {KEY_T, 0},
        {KEY_O, 0},

        {KEY_SPACE, 0},

        {KEY_F, 0},
        {KEY_I, 0},
        {KEY_X, 0},

        {KEY_SPACE, 0},

        {KEY_U, 0},
        {KEY_N, 0},
        {KEY_D, 0},
        {KEY_E, 0},
        {KEY_F, 0},
        {KEY_I, 0},
        {KEY_N, 0},
        {KEY_E, 0},
        {KEY_D, 0},

        {KEY_SPACE, 0},

        {KEY_I, 0},
        {KEY_S, 0},

        {KEY_SPACE, 0},

        {KEY_N, 0},
        {KEY_O, 0},
        {KEY_T, 0},

        {KEY_SPACE, 0},

        {KEY_A, 0},

        {KEY_SPACE, 0},

        {KEY_F, 0},
        {KEY_U, 0},
        {KEY_N, 0},
        {KEY_C, 0},
        {KEY_T, 0},
        {KEY_I, 0},
        {KEY_O, 0},
        {KEY_N, 0},

    };

    uint8_t size = sizeof(keys) / sizeof(keys[0]);
    for (uint8_t i = 0; i < size; i++)
    {
        usb_keyboard_press(keys[i][0], keys[i][1]);
        _delay_ms(20);
    }

    _delay_ms(3000); // wait for spotlight display options

    // web search is second options from the end
    for (size_t i = 0; i < 20; i++)
    {
        usb_keyboard_press(KEY_DOWN, 0);
    }

    usb_keyboard_press(KEY_UP, 0);
    usb_keyboard_press(KEY_ENTER, 0);
}

int main(int argc, char **argv)
{
    // Set PB4 (green led) as output
    DDRB |= (1 << PINB4);

    // set PF4 PF1 PF5 PF0 as input
    DDRF &= ~(1 << PINF4) | ~(1 << PINF1) | ~(1 << PINF5) | ~(1 << PINF0);

    // activate PF4 PF1 PF5 PF0 pull ups
    PORTF |= (1 << PINF4) | (1 << PINF1) | (1 << PINF5) | (1 << PINF0);

    // set PE2 (HWB) as input
    DDRE &= ~(1 << PINE2);
    // PE2 has external pull-up

    PORTB |= (1 << PINB4);
    usb_init();

    while (!usb_configured())
    {
        _delay_ms(10);
    }
    PORTB &= ~(1 << PINB4);

    // // Wait an extra second for operating system to load drivers
    // // and do whatever it does to actually be ready for input
    _delay_ms(1000);

    uint8_t prev[4] = {0, 0, 0, 0};
    uint8_t need_to_update = 0;
    while (1)
    {
        // sw 2
        if (bit_is_clear(PINE, PINE2)) // button pressed

        {
            _delay_ms(40); // Debounce
            if (bit_is_clear(PINE, PINE2))
            {
                send_shrug();
            }
        }

        uint8_t pins[][3] = {
            {PINF4, KEY_LEFT_BRACE, KEY_SHIFT},
            {PINF1, KEY_RIGHT_BRACE, KEY_SHIFT},
            {PINF5, KEY_SEMICOLON, 0},
            {PINF0, KEY_ENTER, 0}};

        uint8_t size = sizeof(pins) / sizeof(pins[0]);

        for (uint8_t i = 0; i < size; i++)
        {
            if (bit_is_clear(PINF, pins[i][0]))

            {
                _delay_ms(40); // Debounce
                if (bit_is_clear(PINF, pins[i][0]))
                {
                    if (prev[i] != 1)
                    {
                        prev[i] = 1;
                        keyboard_keys[i] = pins[i][1];
                        keyboard_modifier_keys = pins[i][2];
                        need_to_update = 1;
                        break;
                    }
                }
            }
            else
            {
                if (prev[i] != 0)
                {
                    prev[i] = 0;
                    keyboard_keys[i] = 0;
                    keyboard_modifier_keys = 0;
                    need_to_update = 1;
                    break;
                }
            }
        }

        if (need_to_update != 0)
        {
            usb_keyboard_send();
            need_to_update = 0;
        }
    }
}
