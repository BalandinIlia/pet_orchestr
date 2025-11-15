#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "netdb.h"
#include "cstring"
#include "thread"
#include "../logger/logger.h"
#include "make_socket.h"

static SOCKET listenPort(TCPPort port)
{
    const SOCKET idSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    bind(idSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

    listen(idSocket, SOMAXCONN);

    return idSocket;
}

SOCKET listenInfo()
{
    return listenPort(portInfo);
}

void CInteractKuberentes::start()
{
    std::thread t(informLive);
    t.detach();
    log("Kubernetes interaction started");
}

void CInteractKuberentes::terminateLive()
{
    LG lk(m_mutLive);
    m_live = false;
    log("Kubernetes liveliness signal terminated");
}

void CInteractKuberentes::informLive()
{
    setThreadName("kubernetes liveliness");
    const SOCKET id = listenPort(portLive);
    log("listening socket created");
    for(;;)
    {
        bool bCon = false;
        {
            LG lk(m_mutLive);
            bCon = m_live;
            log("m_live flag read");
        }
        if(bCon)
        {
            log("waiting for incoming connection");
            accept(id, nullptr, nullptr);
            log("received incoming connection");
        }
        else
            break;
    }
}

bool CInteractKuberentes::m_live = true;
std::mutex CInteractKuberentes::m_mutLive;

SOCKET connectToService()
{
    log("connectToService function started");
    
    const std::string addrDNS(std::getenv("SERVICE"));
    log("DNS address", addrDNS);
    const std::string port(std::getenv("SERVICE_PORT"));
    log("Port", port);
 
    addrinfo constrain{};
    constrain.ai_family = AF_INET;
    constrain.ai_socktype = SOCK_STREAM;
    addrinfo* pRes = nullptr;
    const int resDNS = getaddrinfo(dnsName.c_str(), port.c_str(), &constrain, &pRes);
    if(resDNS != 0)
        log("getaddrinfo returned", resDNS, true);
    if(pRes == nullptr)
        log("Failed to resolve DNS", true);

    const addrinfo& res = *pRes;
    if(res.ai_next != nullptr)
        log("More than 1 resolution", true);

    const SOCKET idSocket = socket(AF_INET, SOCK_STREAM, 0);
    int resCon = connect(idSocket, res.ai_addr, sizeof(res.ai_addr));
    if(resCon != 0)
        log("connect returned " + resCon, true);

    log("connectToService function finished");
    return idSocket;
}