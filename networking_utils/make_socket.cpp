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
    LOG1("Kubernetes interaction started")
}

void CInteractKuberentes::terminateLive()
{
    LG lk(m_mutLive);
    m_live = false;
    LOG1("Kubernetes liveliness signal terminated")
}

void CInteractKuberentes::informLive()
{
    setThreadName("kubernetes liveliness");
    const SOCKET id = listenPort(portLive);
    LOG1("listening socket created")
    for(;;)
    {
        bool bCon = false;
        {
            LG lk(m_mutLive);
            bCon = m_live;
            LOG1("m_live flag read")
        }
        if(bCon)
        {
            LOG1("waiting for incoming connection")
            accept(id, nullptr, nullptr);
            LOG1("received incoming connection")
        }
        else
            break;
    }
}

bool CInteractKuberentes::m_live = true;
std::mutex CInteractKuberentes::m_mutLive;

SOCKET connectToService()
{
    LOG1("connectToService function started")
    
    const std::string nameDNS(std::getenv("SERVICE"));
    LOG2("DNS address", nameDNS)
    const std::string port(std::getenv("SERVICE_PORT"));
    LOG2("Port", port)
 
    addrinfo constrain{};
    constrain.ai_family = AF_INET;
    constrain.ai_socktype = SOCK_STREAM;
    addrinfo* pRes = nullptr;
    const int resDNS = getaddrinfo(nameDNS.c_str(), port.c_str(), &constrain, &pRes);
    if(resDNS != 0)
        LOG3("getaddrinfo returned", resDNS, true)
    if(pRes == nullptr)
        LOG2("Failed to resolve DNS", true)

    const addrinfo& res = *pRes;
    if(res.ai_next != nullptr)
        LOG2("More than 1 resolution", true)

    const SOCKET idSocket = socket(AF_INET, SOCK_STREAM, 0);
    const int resCon = connect(idSocket, res.ai_addr, res.ai_addrlen);
    if(resCon != 0)
        LOG3("connect returned ", resCon, true)

    LOG1("connectToService function finished")
    return idSocket;
}