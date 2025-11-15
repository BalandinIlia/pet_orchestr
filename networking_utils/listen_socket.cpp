#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "cstring"
#include "thread"
#include "../logger/logger.h"
#include "listen_socket.h"

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
    for(;;)
    {
        bool bCon = false;
        {
            LG lk(m_mutLive);
            bCon = m_live;
        }
        if(bCon)
        {
            const SOCKET id = listenPort(portLive);
            accept(id, nullptr, nullptr);
            log("Liveliness confirmed to Kubernetes");
        }
        else
            break;
    }
}

bool CInteractKuberentes::m_live = true;
std::mutex CInteractKuberentes::m_mutLive;