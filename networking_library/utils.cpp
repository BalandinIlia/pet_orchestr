#include "pch.h"
#include "utils.h"

bool recvAll(int id, char* buf, int len)
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

bool sendAll(int id, char* buf, int len)
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