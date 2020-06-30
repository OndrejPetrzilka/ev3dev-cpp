#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

namespace ev3api
{
    class EV3Debug
    {
    public:
        int printLine(const char *format, ...)
        {
            va_list args;
            va_start(args, format);
            int r = vfprintf(stderr, format, args);
            fprintf(stderr, "\n");
            va_end(args);
            return r;
        }

        int print(const char *format, ...)
        {
            va_list args;
            va_start(args, format);
            int r = vfprintf(stderr, format, args);
            va_end(args);
            return r;
        }
    };

    EV3Debug debug;

    // convert a string represenation of an IP address into its numeric equivalent
    uint32_t Inet_AtoN(const char *buf)
    {
        // net_server inexplicably doesn't have this function; so I'll just fake it
        uint32_t ret = 0;
        int shift = 24; // fill out the MSB first
        bool startQuad = true;
        while ((shift >= 0) && (*buf))
        {
            if (startQuad)
            {
                unsigned char quad = (unsigned char)atoi(buf);
                ret |= (((uint32_t)quad) << shift);
                shift -= 8;
            }
            startQuad = (*buf == '.');
            buf++;
        }
        return ret;
    }

    // Reports values through UDP Broadcast on port 9999
    // Format:
    //    float value (4B)
    //    char[] id (until end of packet, UTF8 characters)
    void reportData(const char *id, float value)
    {
        static int sock = 0;

        struct sockaddr_in s = {};
        s.sin_family = AF_INET;
        s.sin_port = (in_port_t)htons(9999);
        s.sin_addr.s_addr = htonl(INADDR_BROADCAST);

        if (sock == 0)
        {
            sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock < 0)
            {
                fprintf(stderr, "Error opening socket");
                return;
            }
            int broadcastEnable = 1;
            int ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
            sendto(sock, nullptr, 0, 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in));
        }

        // TODO: Would be nice to add message coalescing

        int idLen = strlen(id);
        char buff[idLen + 5] = {};

        *(float*)buff = value;
        strcpy(&buff[4], id);

        if (sendto(sock, buff, idLen + 4, 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) < 0)
            fprintf(stderr, "Error sending data to socket");
    }
} // namespace ev3api