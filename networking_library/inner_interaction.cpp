#include "inner_interaction.h"
#include "../networking_utils/send_receive.h"
#include "../logger/logger.h"

namespace IC
{
    std::optional<std::vector<number>> ask(const SOCK& id, number num)
    {
        LOG2("Inner communication: asking service for number", num)
        
        if(!sendNum(id, num))
        {
            LOG2("Failed to send a number", true)
            return std::nullopt;
        }
        
        std::vector<number> ans;
        for(;;)
        {
            const std::optional<number> num = recvNum(id);
            if(num == std::nullopt)
            {
                LOG2("Failed to receive a number", true)
                return std::nullopt;
            }
            else if(num.value() == 0)
                break;
            else
                ans.push_back(num.value());
        }
        return ans;
    }

    bool answer(const SOCK& id, const std::vector<number>& ans)
    {
        bool bOk = true;
        for(const number& n: ans)
            bOk &= sendNum(id, n);
        bOk &= sendNum(id, 0);
        return bOk;
    }

    std::optional<number> getReq(const SOCK& id)
    {
        return recvNum(id);
    }
}