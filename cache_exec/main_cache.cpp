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
	const std::optional<SOCK> sockServ = connectToService();
    if(sockServ == std::nullopt)
	{
		LOG2("Failed to connect to service", true)
		return std::nullopt;
	}
	else
	{
		LOG2("Connected to service. Service socket id:", sockServ.value())
	}

	const std::optional<std::vector<number>> aNum = askInner(sockServ.value(), num);
    
    if(aNum != std::nullopt)
        LOG2("Received an answer from calc service:", aNum.value())
    else
        LOG2("Failed to get an answer from calc service", true)

    return aNum;
}

static void solveReq(SOCK&& sockReq)
{
    CThreadName tn("Solve request thread");

    const std::optional<number> reqNum = getReqInner(sockReq);
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
    if(!answerInner(sockReq, ans))
        LOG2("Failed to send the answer", true)
}

int main()
{
    CThreadName tn("Main thread");
    LOG1(std::string("Starting cache container"));

    CInteractKuberentes::start();

    const std::optional<SOCK> sockMain = listenInfo();
    if(sockMain.has_value())
        LOG1("Main socket created")
    else
    {
        LOG2("Failed to create main socket", true)
        CInteractKuberentes::terminateLive();
    }

    for (;;)
    {
        SOCK sockReq = sockMain.value().acceptS();
        LOG1(std::string("New request received"));
        std::thread t(solveReq, sockReq.mv());
        t.detach();
    }

    return 0;
}