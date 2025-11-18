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
	log("Sending to calc service number ", num);

	const SOCKET idSocketService = connectToService();
	log("Service socket id", idSocketService);

	std::vector<number> aNum = askInner(idSocketService, num);
    log("Received an answer from calc service. First number is ", aNum[0]);

    return aNum;
}

static void solveReq(SOCKET id)
{
    setThreadName("Solve request thread");

    const number reqNum = getReqInner(id);
    
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
        answerInner(id, ans);
    else
    {
        ans = askCalc(reqNum);
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
    log(std::string("Starting cache container"));

    CInteractKuberentes::start();

    const SOCKET idSocket = listenInfo();
    log(std::string("Main socket created"));

    for (;;)
    {
        SOCKET conn = accept(idSocket, nullptr, nullptr);
        log(std::string("New request received"));
        std::thread t(solveReq, conn);
        t.detach();
    }

    return 0;
}