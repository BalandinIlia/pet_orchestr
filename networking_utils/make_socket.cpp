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

class CSOCKFactory
{
public:
    static SOCK make(SOCKET id)
    {
        SOCK ans;
        ans.m_id = id;
        return std::move(ans);
    }
};

SOCK::SOCK(): m_id(-1) {}

SOCK::SOCK(SOCK&& inst)
{
    m_id = inst.m_id;
    inst.m_id = -1;
}

SOCK& SOCK::operator=(SOCK&& inst)
{
    m_id = inst.m_id;
    inst.m_id = -1;
    return *this;
}

SOCK SOCK::acceptS(const SOCK& s) const
{
    SOCKET t = s;
    SOCKET acc = accept(t, nullptr, nullptr);
    SOCK ans;
    ans.m_id = acc;
    return std::move(ans);
}

SOCK::~SOCK()
{
    if(m_id >= 0)
        close(m_id);
}

static std::optional<SOCK> listenPort(TCPPort port)
{
    const SOCKET idSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (idSocket <= 0)
    {
        LOG3("Failed to create a listening port (socket function) at", port, true);
        return std::nullopt;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    if(bind(idSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) != 0)
    {
        LOG3("Failed to create a listening port (bind function) at", port, true);
        return std::nullopt;
    }

    if(listen(idSocket, SOMAXCONN) != 0)
    {
        LOG3("Failed to create a listening port (listen function) at", port, true);
        return std::nullopt;
    }

    return CSOCKFactory::make(idSocket);
}

std::optional<SOCK> listenInfo()
{
    return listenPort(portInfo);
}

SOCK sockClient()
{
    SOCKET idSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serverAddr.sin_port = htons(portInfo);
    connect(idSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

    return CSOCKFactory::make(idSocket);
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
    
    const std::optional<SOCK> id = listenPort(portLive);
    if(id.has_value())
        LOG1("listening socket created")
    else
        LOG2("Failed to create listening socket", true)
    
    for(;;)
    {
        bool bCon = false;
        {
            LG lk(m_mutLive);
            bCon = m_live;
        }
        if(bCon)
        {
            accept(id.value(), nullptr, nullptr);
            LOG1("received incoming connection")
        }
        else
            break;
    }
}

bool CInteractKuberentes::m_live = true;
std::mutex CInteractKuberentes::m_mutLive;

SOCK connectToService()
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
    return CSOCKFactory::make(idSocket);
}