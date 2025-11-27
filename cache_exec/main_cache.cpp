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

static std::optional<std::vector<number>> askCalc(number num)
{
	const SOCK sockServ = connectToService();
	LOG2("Service socket id", sockServ)

	const std::optional<std::vector<number>> aNum = askInner(sockServ, num);
    
    if(aNum != std::nullopt)
        LOG2("Received an answer from calc service. First number is ", aNum.value()[0])
    else
        LOG2("Failed to get an answer from calc service", true)

    return aNum;
}

static void solveReq(SOCK&& sockReq)
{
    setThreadName("Solve request thread");

    const std::optional<number> reqNum = getReqInner(id);
    if(reqNum == std::nullopt)
    {
        LOG2("Failed to receive request value", true)
        return;
    }
    LOG2("Request ", reqNum.value())
    
    std::vector<number> ans;

    bool bFound = false;
    {
        std::shared_lock lk(mutCache);
        if(cache.find(reqNum.value()) != cache.end())
        {
            ans = cache[reqNum.value()];
            bFound = true;
        }
    }
    
    if(bFound)
        LOG2("Answer found in cache:", ans)
    else
    {
        LOG1("Answer not found in cache; adressing calc service")
        std::optional<std::vector<number>> res = askCalc(reqNum.value());
        if(res == std::nullopt)
        {
            LOG2("Failed to receive an answer from calc service", true)
            return;
        }
        else
        {
            ans = res.value();
            LOG2("Answer received:", ans)
            std::unique_lock lk(mutCache);
            cache[reqNum.value()] = ans;
        }
    }
    if(!answerInner(id, ans))
        LOG2("Failed to send the answer", true)
}

int main()
{
    setThreadName("Main thread");
    LOG1(std::string("Starting cache container"));

    CInteractKuberentes::start();

    const std::optional<SOCK> sockMain = listenInfo();
    if(sockMain.has_value())
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
        SOCK sockReq = sockMain.acceptS();
        LOG1(std::string("New request received"));
        std::thread t(solveReq, std::move(sockReq));
        t.detach();
    }

    return 0;
}