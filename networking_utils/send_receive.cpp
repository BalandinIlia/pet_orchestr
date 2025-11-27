#include "array"
#include "vector"
#include "map"
#include "mutex"
#include "../logger/logger.h"
#include "send_receive.h"

bool recvAll(SOCK id, char* buf, int len)
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

bool sendAll(SOCK id, char* buf, int len)
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

bool sendNum(SOCK id, number num)
{
    char* p = reinterpret_cast<char*>(&num);
    return sendAll(id, p, 8);
}

std::optional<number> recvNum(SOCK id)
{
    number num = 0;
    char* p = reinterpret_cast<char*>(&num);
    const bool bSuc = recvAll(id, p, 8);
    if(!bSuc)
        LOG2("Failed to receive a number", true)
    return bSuc ? std::optional<number>(num) : std::nullopt;
}