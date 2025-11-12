#include "iostream"
#include "string"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "inner_interaction.h"
#include "../networking_utils/send_receive.h"
#include "../logger/logger.h"

std::vector<number> askInner(const std::string& dnsName, number num)
{
    log("Starting function askInner");
    log("Start resolving DNS name" + dnsName);
    
    addrinfo* pSysAddr;
    const int resDNS = getaddrinfo(dnsName.c_str(), nullptr, nullptr, &pSysAddr);
    if(resDNS != 0)
        log("getaddrinfo returned " + resDNS, true);
    if(pSysAddr == nullptr)
        log("Failed to resolve DNS", true);

    addrinfo& sys = *pSysAddr;
    if(sys.ai_next != nullptr)
        log("More than 1 resolution", true);

    const SOCKET idSocket = socket(AF_INET, SOCK_STREAM, 0);
    int resCon = connect(idSocket, sys.ai_addr, sizeof(sys.ai_addr));
    if(resCon != 0)
        log("connect returned " + resCon, true);

    log("Sending data");
    sendAll(idSocket, reinterpret_cast<char*>(num), 8);
    log("Data sent");

    log("Receiving data");
    std::vector<number> ans;
    for(;;)
    {
        number cur = 0;
        recvAll(idSocket, reinterpret_cast<char*>(cur), 8);
        if(cur == 0)
            break;
        else
            ans.push_back(cur);
    }
    log("Data received");

    log("Finishing function askInner");
    return ans;
}