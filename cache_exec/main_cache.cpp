#include "thread"
#include "mutex"
#include "shared_mutex"
#include "string"
#include "sstream"
#include "map"
#include "../networking_library/messages.h"
#include "../networking_library/inner_interaction.h"
#include "../networking_utils/send_receive.h"
#include "../networking_utils/make_socket.h"
#include "../logger/logger.h"

static std::map<number, std::vector<number>> cache;
static std::shared_mutex mutCache;

static std::vector<number> askCalc(number num)
{
	const SOCKET idSocketService = connectToService();
	LOG2("Service socket id", idSocketService)

	std::vector<number> aNum = askInner(idSocketService, num);
    LOG2("Received an answer from calc service. First number is ", aNum[0])

    return aNum;
}

static void solveReq(SOCKET id)
{
    setThreadName("Solve request thread");

    const number reqNum = getReqInner(id);
    LOG2("Request ", reqNum)
    
    std::vector<number> ans;

    bool bFound = false;
    {
        std::shared_lock lk(mutCache);
        if(cache.find(reqNum) != cache.end())
        {
            ans = cache[reqNum];
            bFound = true;
        }
    }
    
    if(bFound)
    {
        LOG2("Answer found in cache:", ans)
        answerInner(id, ans);
    }
    else
    {
        LOG1("Answer not found in cache; adressing calc service")
        ans = askCalc(reqNum);
        LOG2("Answer received:", ans)
        {
            std::unique_lock lk(mutCache);
            cache[reqNum] = ans;
        }
        answerInner(id, ans);
    }
}

int main()
{
    setThreadName("Main thread");
    LOG1(std::string("Starting cache container"));

    CInteractKuberentes::start();

    const std::optional<SOCKET> idSocket = listenInfo();
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

    for (;;)
    {
        SOCKET conn = accept(idSocket.value(), nullptr, nullptr);
        LOG1(std::string("New request received"));
        std::thread t(solveReq, conn);
        t.detach();
    }

    return 0;
}