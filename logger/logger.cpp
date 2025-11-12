#include "map"
#include "shared_mutex"
#include "mutex"
#include "iostream"
#include "sstream"
#include "thread"
#include "logger.h"

class CThreadInfo
{
public:
    static void registerThisThread(const std::string& name)
    {
        std::unique_lock l(m_mut);

        int maxNum = 0;
        for(auto& iter: m_map)
        {
            const std::string& curName = iter.second.first;
            const int& curNum = iter.second.second;
            if(curName == name)
                maxNum = std::max(maxNum, curNum);
        }
        maxNum++;
        
        const std::thread::id idThread = std::this_thread::get_id();
        m_map[idThread] = std::pair<std::string, int>(name, maxNum);
    }

    static std::pair<std::string, int> threadInfo()
    {
        std::shared_lock l(m_mut);
        
        const std::thread::id idThread = std::this_thread::get_id();
        if(m_map.find(idThread) != m_map.end())
            return m_map[idThread];
        else
            return std::pair<std::string, int>("no_name", -1);
    }

private:
    static std::map<std::thread::id, std::pair<std::string, int>> m_map;

    static std::shared_mutex m_mut;
};

static std::mutex mutCons;

void log(const std::string& log)
{
    const std::pair<std::string, int> infoThr = CThreadInfo::threadInfo();

    std::ostringstream mes;
    mes << infoThr.first;
    mes << " ";
    mes << "thread number";
    mes << " ";
    mes << infoThr.second;
    mes << ": ";
    mes << log;

    LG lk(mutCons);
    std::cout << mes.str() << std::endl;
}

void setThreadName(const std::string& name)
{
    CThreadInfo::registerThisThread(name);
}