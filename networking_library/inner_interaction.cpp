#include "iostream"
#include "string"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "inner_interaction.h"
#include "../networking_utils/send_receive.h"
#include "../logger/logger.h"

std::vector<number> askInner(SOCKET id, number num)
{
    LOG1("Starting function askInner");
    sendNum(id, num);
    number cur = 0;
    std::vector<number> ans;
    for(;;)
    {
        recvNum(id, cur);
        if(cur == 0)
            break;
        else
            ans.push_back(cur);
    }
    return ans;
}

void answerInner(SOCKET id, std::vector<number> ans)
{
    for(const number& n: ans)
        sendNum(id, n);
    sendNum(id, 0);
}

number getReqInner(SOCKET id)
{
    number ans = 0;
    recvNum(id, ans);
    return ans;
}