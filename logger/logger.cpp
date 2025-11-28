#include "map"
#include "shared_mutex"
#include "mutex"
#include "iostream"
#include "sstream"
#include "thread"
#include "logger.h"

// This class stores information associated with threads. For each thread there is the following information:
// 1. Thread name
// 2. Thread number (to distinguish between threads with the same name)
// Inside the class threads are identified by their system thread ids.
class CThreadInfo
{
public:
    // This function registers the current thread (the thread which executes this function)
    // in this structure.
    static void registerThisThread(const std::string& name)
    {
        std::unique_lock lk(m_mut);

        // register this thread with the next free number
        const std::thread::id idThread = std::this_thread::get_id();
        // thread number
        const int thrNum = (m_mapNameNum.find(name) == m_mapNameNum.end()) ? 1 : m_mapNameNum[name]+1;        
        m_mapIdData[idThread] = std::pair<std::string, int>(name, thrNum);
        m_mapNameNum[name] = thrNum;
    }

    static void deregisterThisThread()
    {
        std::unique_lock lk(m_mut);
        // register this thread with the next free number
        const std::thread::id idThread = std::this_thread::get_id();
        m_mapIdData.erase(idThread);
    }

    // get info about the current thread
    static std::pair<std::string, int> threadInfo()
    {
        const std::thread::id idThread = std::this_thread::get_id();

        std::shared_lock l(m_mut);
        if(m_mapIdData.find(idThread) != m_mapIdData.end())
            return m_mapIdData[idThread];
        else
            return std::pair<std::string, int>("no_name", -1);
    }

private:
    // registered threads
    // thread system id -> (thread name, thread number)
    static std::map<std::thread::id, std::pair<std::string, int>> m_mapIdData;

    // thread name -> last number used for the thread
    static std::map<std::string, int> m_mapNameNum;

    // mutex protecting the maps
    static std::shared_mutex m_mut;
};

std::map<std::thread::id, std::pair<std::string, int>> CThreadInfo::m_mapIdData;
std::map<std::string, int> CThreadInfo::m_mapNameNum;
std::shared_mutex CThreadInfo::m_mut;

// Mutex protecting the console (logs are outputted to the console)
static std::mutex mutCons;

// Inner function, which implements logging
static void logImpl(const char* fn, const std::string& log, bool bError)
{
    const std::pair<std::string, int> infoThr = CThreadInfo::threadInfo();

    // mes = "message"
    std::ostringstream mes;
    
    if(bError)
        mes << "ERROR";
    else
        mes << "INFO";
    mes << "\n";

    mes << "      ";
    mes << "Thread: ";
    mes << infoThr.first;
    mes << " ";
    mes << infoThr.second;
    mes << "\n";

    mes << "      ";
    mes << "Function: ";
    mes << fn;
    mes << "\n";

    mes << "      ";
    mes << "Message: ";
    mes << log;

    const std::string out = mes.str();
    LG lk(mutCons);
    std::cout << out << std::endl;
}

void log(const char* fn, const std::string& log, bool bError)
{
    logImpl(fn, log, bError);
}

void log(const char* fn, const std::string& s1, const std::string& s2, bool bError)
{
    // mes = "message"
    std::ostringstream mes;
    mes << s1;
    mes << " ";
    mes << s2;
    logImpl(fn, mes.str(), bError);
}

void log(const char* fn, const std::string& s, int v, bool bError)
{
    // mes = "message"
    std::ostringstream mes;
    mes << s;
    mes << " ";
    mes << v;
    logImpl(fn, mes.str(), bError);
}

void log(const char* fn, const std::string& s1, number n, bool bError)
{
    // mes = "message"
    std::ostringstream mes;
    mes << s1;
    mes << " ";
    mes << n;
    logImpl(fn, mes.str(), bError);
}

void log(const char* fn, const std::string& s1, const std::vector<number>& v, bool bError)
{
    // mes = "message"
    std::ostringstream mes;
    mes << s1;
    for(number i: v)
    {
        mes << " ";
        mes << i;
    }
    logImpl(fn, mes.str(), bError);
}

CThreadName::CThreadName(const std::string& name)
{
    CThreadInfo::registerThisThread(name);
}

CThreadName::~CThreadName()
{
    CThreadInfo::deregisterThisThread();
}