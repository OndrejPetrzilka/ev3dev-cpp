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
#include <map>

using namespace std;

void queueMessge(const void *ptr, int size)
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

    if (sendto(sock, ptr, size, 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) < 0)
        fprintf(stderr, "Error sending data to socket");
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
void reportData(const char *id, float value)
{
    static map<string, int> idLookup;

    string idStr(id);

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

        int buffSize = 8 + idStr.size();

        char buff[buffSize];
        *(float *)buff = value;
        buff[4] = 255;
        buff[5] = (char)valueIndex;
        buff[6] = idStr.size();
        strcpy(&buff[7], idStr.c_str());

        queueMessge(buff, buffSize);
    }
    else
    {
        valueIndex = it->second;

        char buff[5];
        *(float *)buff = value;
        buff[4] = (char)valueIndex;

        queueMessge(buff, 5);
    }
}