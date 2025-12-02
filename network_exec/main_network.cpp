#include "thread"
#include "string"
#include "sstream"
#include "functional"
#include "../networking_library/messages.h"
#include "../networking_utils/send_receive.h"
#include "../networking_utils/make_socket.h"
#include "../logger/logger.h"
#include "clientInstance.h"

int main()
{
    CThreadName tn("Main thread");
    LOG1(std::string("Starting networking container"))

    CInteractKuberentes::start();

    const std::optional<SOCK> idSocket = listenInfo();
    if(idSocket.has_value())
        LOG1("Main socket created")
    else
    {
        LOG2("Failed to create main socket", true)
        CInteractKuberentes::terminateLive();
    }

    // client identificator
    // Receive a connection from a client and serve the client in a separate thread. 
    // This thread keeps listening for new connections. This design allows to serve
    // several clients simultaneously.
    int idClient = 1;
    for (;;)
    {
        SOCK conn = idSocket.value().acceptS();
        LOG1(std::string("New client accepted"));
        std::thread t(serveClient, conn.mv(), idClient);
        idClient++;
        t.detach();
    }

    return 0;
}