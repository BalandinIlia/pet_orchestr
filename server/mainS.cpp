#include "thread"
#include "string"
#include "sstream"
#include "../networking_library/messages.h"
#include "../networking_utils/send_receive.h"
#include "../networking_utils/listen_socket.h"
#include "../logger/logger.h"
#include "clientInstance.h"

int main()
{
    setThreadName("Main thread");
    log(std::string("Starting networking container"));

    const SOCKET idSocket = listenInfo();
    log(std::string("Main socket created"));

    // client identificator
    // Receive a connection from a client and serve the client in a separate thread. 
    // This thread keeps listening for new connections. This design allows to serve
    // several clients simultaneously.
    int idClient = 1;
    for (;;)
    {
        int conn = accept(idSocket, nullptr, nullptr);
        log(std::string("New client accepted"));
        std::thread t(serveClient, conn, idClient);
        idClient++;
        t.detach();
    }

    return 0;
}
