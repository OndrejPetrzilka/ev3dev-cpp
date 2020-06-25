#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LED_LEFT_GREEN "/sys/class/leds/ev3:left:green:ev3dev"

namespace ev3api
{
    class EV3Led
    {
    private:
        FILE *m_brightness;
        FILE *m_maxBrightness;
        FILE *m_delayOff;
        FILE *m_delayOn;
        FILE *m_trigger;

    public:
        EV3Led(const char *sysDev)
        {
        }

        ~EV3Led()
        {
        }
    };

    class EV3Leds
    {
    private:
        const char *const DEVICE = "/sys/class/leds/";

    public:
        EV3Leds()
        {
        }

        ~EV3Leds()
        {
        }

        EV3Led leftGreen()
        {
        }
    };

    EV3Leds leds;
} // namespace ev3api