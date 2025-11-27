#include "thread"
#include "string"
#include "sstream"
#include "../networking_library/messages.h"
#include "../networking_utils/send_receive.h"
#include "../networking_utils/make_socket.h"
#include "../logger/logger.h"
#include "unistd.h"
#include "clientInstance.h"

int main()
{
    setThreadName("Main thread");
    LOG1(std::string("Starting networking container"))

    CInteractKuberentes::start();

    const std::optional<SOCK> idSocket = listenInfo();
    if(idSocket.has_value())
        LOG1("Main socket created")
    else
    {
        LOG2("Failed to create main socket", true)
        // sleep for an hour to keep the pos alive; This enables the operator to see the error:
        // logs are removed when the pod crashes
        sleep(3600);
        throw std::exception();
    }

    // client identificator
    // Receive a connection from a client and serve the client in a separate thread. 
    // This thread keeps listening for new connections. This design allows to serve
    // several clients simultaneously.
    int idClient = 1;
    for (;;)
    {
        SOCK conn = idSocket.accept();
        LOG1(std::string("New client accepted"));
        std::thread t(serveClient, conn, idClient);
        idClient++;
        t.detach();
    }

    return 0;
}
