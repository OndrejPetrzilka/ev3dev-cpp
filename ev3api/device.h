#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <sstream>
#include <algorithm>
#include <dirent.h>

#include "ports.h"
#include "attribute/attribute.h"
#include "serializer/serializer.h"

using namespace std;

namespace ev3api
{
    // Base-class for sysfs device
    class Device
    {
        string m_path;

    public:
        const char *const Path;

        Attribute<string> m_address = "address";
        Attribute<string> m_driver_name = "driver_name";

        Device(string devicePath)
            : m_path(devicePath), Path(m_path.c_str())
        {
        }

        const char *getPath() const
        {
            return Path;
        }

        bool isConnected()
        {
            struct stat sb;
            return stat(Path, &sb) == 0 && S_ISDIR(sb.st_mode);
        }

        string getAddress() { return m_address.getValue(Path); }
        string getDriverName() { return m_driver_name.getValue(Path); }

        static string findDevicePath(const char *basePath, const char *port)
        {
            // fprintf(stderr, "FindDevice: %s, %s\n", basePath, port);

            // List all dirs inside "basePath"
            // In each dir try to find out "address" file
            // Check "address" file contains "port"
            DIR *d = opendir(basePath);
            if (d)
            {
                int portLen = strlen(port);
                char buff[portLen + 1];

                struct dirent *dir;
                while ((dir = readdir(d)) != NULL)
                {
                    if (dir->d_name[0] != '.')
                    {
                        string addrPath = basePath;
                        addrPath += dir->d_name;
                        addrPath += "/address";

                        // fprintf(stderr, "Candidate path: %s\n", addrPath.c_str());

                        FILE *addrFile = fopen(addrPath.c_str(), "r");
                        if (addrFile != nullptr)
                        {
                            int read = fread(buff, sizeof(char), portLen + 1, addrFile);
                            fclose(addrFile);

                            if (read < portLen || (read > portLen && buff[portLen] != '\0' && buff[portLen] != '\r' && buff[portLen] != '\n'))
                                continue;

                            buff[portLen] = '\0';
                            // fprintf(stderr, "Content: %s\n", buff);

                            if (strcmp(port, buff) == 0)
                            {
                                string result = basePath;
                                result += dir->d_name;
                                result += "/";
                                //fprintf(stderr, "Returning: %s\n", result.c_str());
                                return result;
                            }
                        }
                    }
                }
                closedir(d);
            }
            fprintf(stderr, "Not found\n");
            return string();
        }

        static string findDevicePath(const char *basePath, InputPort port)
        {
            return findDevicePath(basePath, INPUT_PORTS[(int)port]);
        }

        static string findDevicePath(const char *basePath, OutputPort port)
        {
            return findDevicePath(basePath, OUTPUT_PORTS[(int)port]);
        }
    };
} // namespace ev3api