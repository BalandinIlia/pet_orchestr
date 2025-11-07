#include "winsock2.h"
#include "thread"
#include "string"
#include "sstream"
#include "../networking/messages.h"
#include "../networking/utils.h"
#include "logger.h"
#include "clientInstance.h"

int main()
{
    CWSAConfig conf;

    log(std::string("Server"));

    SOCKET idSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (idSocket == INVALID_SOCKET)
    {
        std::ostringstream mes;
        mes << "Socket creation failed: " << WSAGetLastError();
        log(mes.str());
        Sleep(100000);
        return 0;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(addrIP);
    serverAddr.sin_port = htons(port);
    bind(idSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

    listen(idSocket, SOMAXCONN);

    // client identificator
    // Receive a connection from a client and serve the client in a separate thread. 
    // This thread keeps listening for new connections. This design allows to serve
    // several clients simultaneously.
    int idClient = 1;
    for (;;)
    {
        SOCKET conn = accept(idSocket, nullptr, nullptr);
        std::thread t(serveClient, conn, idClient);
        idClient++;
        t.detach();
    }

    return 0;
}
