#include "thread"
#include "../networking_library/messages.h"
#include "../networking_library/inner_interaction.h"
#include "../networking_utils/send_receive.h"
#include "../networking_utils/make_socket.h"
#include "../logger/logger.h"

static std::vector<number> doCalc(number num)
{
    LOG2("Calcing for ", num);

    if(num == 42)
        throw std::exception();

    std::vector<number> ans;
    for(number d = 2; d < num; d++)
    {
        if(num % d == 0)
            ans.push_back(d);
    }
    LOG2("Quantity of found divisors ", static_cast<number>(ans.size()))
    return ans;
}

static void solveReq(SOCK&& id)
{
    CThreadName tn("Solve request thread");
    const std::optional<number> reqNum = getReqInner(id);
    if(reqNum == std::nullopt)
    {
        LOG2("Failed to receive request number", true)
        return;
    }
    LOG2("Received number ", reqNum.value())
    std::vector<number> ans = doCalc(reqNum.value());
    ans.push_back(0);
    answerInner(id, ans);
}

int main()
{
    CThreadName tn("Main thread");
    LOG1(std::string("Starting calc container"))

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
        SOCK conn = sockMain.value().acceptS();
        LOG1(std::string("New request received"))
        std::thread t(solveReq, conn.mv());
        t.detach();
    }

    return 0;
}