#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "device.h"
#include "attribute/attribute.h"
#include "serializer/serializerTextInt.h"

using namespace std;

namespace ev3api
{
    // Base-class for Motor
    class Motor : public Device
    {
    public:
        AttributeRaw m_state = "state";

        Command<const char *> m_command = "command";

    public:
        Motor(const char *devicePath)
            : Device(devicePath)
        {
        }

        bool setCommand(const char *command) { return m_command.setValue(Path, command); }

        string getStateString() { return m_state.getValue<string>(Path); }
    };
} // namespace ev3api