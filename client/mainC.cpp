#include "iostream"
#include "../networking_library/messages.h"
#include "../networking_utils/send_receive.h"
#include "runner.h"

int main()
{
    // Tell the user they are running the client
    std::cout << "Client" << std::endl;

    SOCK s = sockClient()
    CRunner r(std::move(s));
    r.run();

    return 0;
}