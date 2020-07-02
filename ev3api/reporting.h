#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>

#include "timeUtil.h"

using namespace std;

void queueMessage(const void *ptr, int size, bool flush)
{
    const int mtu = 1200;
    static int sock = 0;
    static int dataSize = 0;
    static unsigned char sendBuff[mtu];
    static Time age;

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

    if (size > 0)
    {
        if (dataSize + size > mtu)
        {
            // Buffer full
            if (sendto(sock, sendBuff, dataSize, 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) < 0)
                fprintf(stderr, "Error sending data to socket");

            dataSize = 0;
        }

        if (dataSize == 0)
        {
            age = Time::now();
        }

        memcpy(&sendBuff[dataSize], ptr, size);
        dataSize += size;
    }

    if (dataSize > 0 && (flush || age.elapsedMiliseconds() > 100))
    {
        if (sendto(sock, sendBuff, dataSize, 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) < 0)
            fprintf(stderr, "Error sending data to socket");

        dataSize = 0;
    }
}

// Reports values through UDP Broadcast on port 9999
// Format:
//    float value (4B)
//    byte valueIndex 0-254
// Or format:
//    float value (4B)
//    byte 255 (1B)
//    byte valueIndex (1B)
//    byte idLength (1B)
//    char[] id (1B * idLength), UTF8, not terminated
void reportData(const char *id, float value, bool flush = false)
{
    if (id == nullptr)
    {
        queueMessage(nullptr, 0, flush);
        return;
    }

    static map<string, int> idLookup;

    string idStr(id);
    int idLen = idStr.size();
    if (idLen > 64)
        idLen = 64;

    // TODO: Resend all ids once every few seconds

    int valueIndex = 0;
    auto it = idLookup.find(idStr);
    if (it == idLookup.end())
    {
        // Full
        if (idLookup.size() >= 254)
        {
            fprintf(stderr, "More than 254 different variables reported");
            return;
        }

        valueIndex = idLookup.size();
        idLookup[idStr] = valueIndex;

        int buffSize = 7 + idLen;

        char buff[buffSize];
        *(float *)buff = value;
        buff[4] = 255;
        buff[5] = (char)valueIndex;
        buff[6] = idLen;
        memcpy(&buff[7], idStr.c_str(), idLen);

        queueMessage(buff, buffSize, flush);
    }
    else
    {
        valueIndex = it->second;

        char buff[5];
        *(float *)buff = value;
        buff[4] = (char)valueIndex;

        queueMessage(buff, 5, flush);
    }
}