#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "device.h"
#include "attribute/attribute.h"
#include "serializer/serializerTextInt.h"

using namespace std;

namespace ev3api
{
    // Base-class for sensor
    class Sensor : public Device
    {
    private:
        template <class T>
        string getDebug()
        {
            int count = getNumValues();
            T data[count];
            std::string result;
            count = m_bin_data.read(Path, data, count);
            for (size_t i = 0; i < count; i++)
            {
                if (i > 0)
                {
                    result += ", ";
                }
                result += to_string(data[i]);
            }
            return result;
        }

    public:
        // TODO: Attribute improvements:
        // 1) Expose them
        // 2) Add override for getter T operator()
        // 3) Add override for setter void operator(T&)
        // 4) Initialize with DevicePath right away (this can be access in member intializers), use something like:
        //    m_mode = { "mode", Path }
        //    m_mode("mode", Path)
        // 5) Add cached attributes with refresh() method, name it Constant
        // 6) Add support for string-based enums
        // 7) Change default type to string or int
        // 8) Change default parsing to conversion from string

        AttributeRaw m_bin_data = "bin_data";
        Attribute<string> m_bin_data_format = "bin_data_format";
        Attribute<string> m_commands = "commands";
        Attribute<int, SerializerTextInt> m_decimals = "decimals";
        Attribute<string> m_fw_version = "fw_version";
        Attribute<string> m_modes = "modes";
        Attribute<int, SerializerTextInt> m_numValues = "num_values";
        Attribute<string> m_units = "units";
        Attribute<string> m_text_value = "text_value";

        VariableRaw m_direct = "direct";
        VariableRaw m_mode = "mode";
        Variable<int> m_poll_ms = "poll_ms";

        Command<const char *> m_command = "command";

    public:
        Sensor(const char *devicePath)
            : Device(devicePath)
        {
        }

        template <class T, class TSerializer = Serializer<T>>
        T getBinData() { return m_bin_data.getValue<T, TSerializer>(Path); }

        string getBinDataFormat() { return m_bin_data_format.getValue(Path); }
        bool setCommand(const char *command) { return m_command.setValue(Path, command); }
        string getCommands() { return m_commands.getValue(Path); }

        template <class T, class TSerializer = Serializer<T>>
        T getDirect(int byteOffset) { return m_direct.getValue<T, TSerializer>(Path, byteOffset); }

        template <class T, class TSerializer = Serializer<T>>
        bool setDirect(const T &value, int byteOffset) { return m_direct.setValue<T, TSerializer>(Path, value, byteOffset); }

        int getDecimals() { return m_decimals.getValue(Path); }
        string getFwVersion() { return m_fw_version.getValue(Path); }
        string getMode() { return m_mode.getValue<string>(Path); }
        bool setMode(const char *mode) { return m_mode.setValue(Path, mode); }
        string getModes() { return m_modes.getValue(Path); }
        int getNumValues() { return m_numValues.getValue(Path); }
        string getUnits() { return m_units.getValue(Path); }
        string getTextValue() { return m_text_value.getValue(Path); }

        int getPollMs() { return m_poll_ms.getValue(Path); }
        bool setPollMs(int value) { return m_poll_ms.setValue(Path, value); }

        string getDebugValues()
        {
            string format = getBinDataFormat();
            if (format == "u8")
                return getDebug<unsigned char>();
            if (format == "s8")
                return getDebug<int8_t>();
            if (format == "u16")
                return getDebug<unsigned short>();
            if (format == "s16")
                return getDebug<short>();
            if (format == "s32")
                return getDebug<int>();
            if (format == "float")
                return getDebug<float>();
            return "unknown type";
        }
    };
} // namespace ev3api