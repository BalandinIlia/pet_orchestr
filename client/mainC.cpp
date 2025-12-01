#include "iostream"
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "../networking_library/messages.h"
#include "../networking_utils/send_receive.h"
#include "runner.h"

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

static SOCK sockClient()
{
    SOCKET idSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serverAddr.sin_port = htons(portInfo);
    connect(idSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

    return CSOCKFactory::make(idSocket);
}

int main()
{
    // Tell the user they are running the client
    std::cout << "Client" << std::endl;

    SOCK s = sockClient();
    CRunner r(std::move(s));
    r.run();

    return 0;
}