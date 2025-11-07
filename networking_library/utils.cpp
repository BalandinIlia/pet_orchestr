#include "pch.h"
#include "utils.h"

#pragma comment(lib, "ws2_32.lib")

CWSAConfig::CWSAConfig()
{
    WSADATA wsaData;
    WSAStartup(0x202, &wsaData);
}

CWSAConfig::~CWSAConfig()
{
    WSACleanup();
}

bool recvAll(SOCKET id, char* buf, int len)
{
    int bytes = 0;
    while (len != 0)
    {
        bytes = recv(id, buf, len, 0);
        if (bytes < 1)
            return false;
        buf += bytes;
        len -= bytes;
        bytes = 0;
    }
    return true;
}

bool sendAll(SOCKET id, char* buf, int len)
{
    int bytes = 0;
    while (len != 0)
    {
        bytes = send(id, buf, len, 0);
        if (bytes < 1)
            return false;
        buf += bytes;
        len -= bytes;
        bytes = 0;
    }
    return true;
}