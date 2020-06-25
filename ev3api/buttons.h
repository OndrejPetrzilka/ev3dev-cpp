#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>

#define KEY_BUF_LEN ((KEY_MAX + 7) / 8)

namespace ev3api
{
    enum Button
    {
        NONE = 0,
        UP = 0x01L,
        DOWN = 0x02L,
        LEFT = 0x04L,
        RIGHT = 0x08L,
        CENTER = 0x10L,
        BACK = 0x20L,
    };

    class EV3Buttons
    {
    private:
        const char *const GPIO_KEYS_PATH = "/dev/input/by-path/platform-gpio_keys-event";

        int m_handle;

        inline bool getKeyBit(unsigned char *keys, int keyCode)
        {
            return keys[keyCode >> 3] & (1 << (keyCode & 7));
        }

        inline int getKeyMask(unsigned char *keys, int keyCode, Button button)
        {
            return (keys[keyCode >> 3] & (1 << (keyCode & 7))) ? button : 0;
        }

    public:
        EV3Buttons()
        {
            m_handle = open(GPIO_KEYS_PATH, O_RDONLY);
            if (m_handle < 0)
            {
                fprintf(stderr, "Error opening input %s", GPIO_KEYS_PATH);
            }
        }

        ~EV3Buttons()
        {
            close(m_handle);
            m_handle = 0;
        }

        bool up()
        {
            return getKeys() & Button::UP;
        }

        bool down()
        {
            return getKeys() & Button::DOWN;
        }

        bool left()
        {
            return getKeys() & Button::LEFT;
        }

        bool right()
        {
            return getKeys() & Button::RIGHT;
        }

        bool center()
        {
            return getKeys() & Button::CENTER;
        }

        bool back()
        {
            return getKeys() & Button::BACK;
        }

        Button getKeys()
        {
            if (m_handle < 0)
                return Button::NONE;

            unsigned char keyb[(KEY_MAX + 7) / 8];
            ioctl(m_handle, EVIOCGKEY(sizeof(keyb)), &keyb);

            int result = 0;
            result |= getKeyMask(keyb, KEY_UP, Button::UP);
            result |= getKeyMask(keyb, KEY_DOWN, Button::DOWN);
            result |= getKeyMask(keyb, KEY_LEFT, Button::LEFT);
            result |= getKeyMask(keyb, KEY_RIGHT, Button::RIGHT);
            result |= getKeyMask(keyb, KEY_ENTER, Button::CENTER);
            result |= getKeyMask(keyb, KEY_BACKSPACE, Button::BACK);
            return (Button)result;
        }
    };
} // namespace ev3api