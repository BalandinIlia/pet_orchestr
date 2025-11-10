#include "iostream"
#include "../networking_library/messages.h"
#include "../networking_library/utils.h"
#include "runner.h"

int main()
{
    // Tell the user they are running the client
    std::cout << "Client" << std::endl;

    int idSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(addrIP);
    serverAddr.sin_port = htons(port);
    connect(idSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

    CRunner r(idSocket);
    r.run();

    return 0;
}