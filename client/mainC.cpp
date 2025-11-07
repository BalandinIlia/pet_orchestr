#include "winsock2.h"
#include "iostream"
#include "../networking/messages.h"
#include "../networking/utils.h"
#include "runner.h"

int main()
{
    CWSAConfig conf;

    // Tell the user they are running the client
    std::cout << "Client" << std::endl;

    SOCKET idSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (idSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << "\n";
        std::cin.get();
        return 0;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(addrIP);
    serverAddr.sin_port = htons(port);
    connect(idSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

    CRunner r(idSocket);
    r.run();

    return 0;
}