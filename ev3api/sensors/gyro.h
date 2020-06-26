#pragma once
#include "sensor.h"
#include "utils.h"
#include <array>

namespace ev3api
{
    // Lego gyroscope, provides data at about 100 Hz
    class Gyro : Sensor
    {
    public:
        enum class Mode
        {
            GYRO_ANG = 0,
            GYRO_RATE = 1,
            GYRO_FAS = 2,
            GYRO_GA = 3,
            GYRO_CAL = 4,
            TILT_RATE = 5,
            TILT_ANG = 6
        };

        Attribute<int, SerializerTextInt> m_value0 = "value0";

        Gyro(const char *devicePath)
            : Sensor(devicePath)
        {
        }

        using Sensor::getDecimals;
        using Sensor::getNumValues;
        using Sensor::getUnits;

        Sensor &getSensor()
        {
            return *this;
        }

        template <class T, class TSerializer = Serializer<T>>
        T getRawValue()
        {
            return Sensor::getBinData<T, TSerializer>();
        }

        template <class T, class TSerializer = Serializer<T>>
        int getRawValues(T* buffer, int count)
        {
            m_bin_data.read<T, TSerializer>(Path, buffer, count, 0);
        }

        int getValue()
        {
            return m_value0.getValue(Path);
        }

        // Calibrates gyro, must be stationary
        void calibrate(float duration = 2)
        {
            auto currentMode = Sensor::getMode();
            setMode(Mode::GYRO_CAL);
            utils.msleep(duration * 1000);
            Sensor::setMode(currentMode.c_str());
        }

        // Resets angle to zero, does not work with multiplexors and other bricks than EV3
        void reset()
        {
            setDirect('\x11', 0);
        }

        static const std::array<const char *, 7> &getModes()
        {
            static const std::array<const char *, 7> modes = {{"GYRO-ANG", "GYRO-RATE", "GYRO-FAS", "GYRO-G&A", "GYRO-CAL", "TILT-RATE", "TILT-ANG"}};
            return modes;
        }

        Mode getMode()
        {
            char buf[16];
            Sensor::m_mode.readLine(Path, buf, 16);
            return modeFromString(buf);
        }

        void setMode(Mode mode)
        {
            Sensor::setMode(modeToString(mode));
        }

        const char *getModeString()
        {
            return modeToString(getMode());
        }

        void setModeString(const char *mode)
        {
            Sensor::setMode(mode);
        }

        const char *modeToString(Mode mode)
        {
            return getModes()[(int)mode];
        }

        Mode modeFromString(const char *modeString)
        {
            auto modes = getModes();
            for (size_t i = 0; i < modes.size(); i++)
            {
                if (strcmp(modes[i], modeString) == 0)
                {
                    return (Mode)i;
                }
            }
            return Mode::GYRO_ANG;
        }
    };
} // namespace ev3api