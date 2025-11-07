#include "runner.h"
#include "sstream"

std::mutex CRunner::m_mutCons;

CRunner::CRunner(SOCKET idS) : m_idSocket(idS), m_id(1) {}

void CRunner::run()
{
    std::thread ts(sendAdapter, this);
    std::thread tr(recvAdapter, this);
    ts.join();
    tr.join();
}

void CRunner::send()
{
    for (;;)
    {
        // 'a' is a signal to receive a number from the user
        char c = 0;
        while (c != 'a')
            c = std::cin.get();

        number num = 0;
        {
            // Take a number from the user (from console). So here I lock the console mutex to own the console.
            LG lk(m_mutCons);
            while (num <= 0)
            {
                std::cout << "Input a number: ";
                std::cin >> num;
                if (num > 0)
                    std::cout << std::endl << "Sent request " << num << ". Request id " << m_id << ".";
                else
                    std::cout << std::endl << "Number must be positive" << std::endl;
            }
        }

        std::array<char, 11> mes = MS::serializeRequest(num, m_id);
        const bool bOk = sendAll(m_idSocket, mes.data(), 11);
        if (!bOk)
        {
            LG lk(m_mutCons);
            std::cout << std::endl << "Connection with server lost";
            return;
        }

        {
            // Modify the table. So here I lock the table mutex to own the table.
            LG lk(m_mutTable);
            m_table[m_id] = num;
            m_tableTime[m_id] = std::chrono::system_clock::now().time_since_epoch();
        }
        
        m_id++;
    }
}

void CRunner::receive()
{
    for (;;)
    {
        char c;
        if (!recvAll(m_idSocket, &c, 1))
        {
            logConnectionLost();
            return;
        }
        const MS::ETypeMes t = MS::decodeType(c);
        switch (t)
        {
        case MS::ETypeMes::eAnsNo:
        {
            std::array<char, 2> buf;
            if(!recvAll(m_idSocket, buf.data(), 2))
            {
                logConnectionLost();
                return;
            }
            const short id = MS::deserializeAnsNo(buf);

            std::ostringstream sError;
            number n = 0;
            TP time = std::chrono::system_clock::now().time_since_epoch();
            {
                // Here I access the table. While I don't modify the table here, I still have to lock the mutex to
                // make sure the table is not modified while I am accessing it. If I don't do this, I can get some 
                // intermediate table state (half-modified).
                LG lk(m_mutTable);
                if (m_table.find(id) != m_table.end())
                {
                    n = m_table[id];
                    time -= m_tableTime[id];
                }
                else
                    sError << "Error: inner table does not contain request id " << id << " received from the server.";
            }

            {
                // Show a number from the user (to console). So here I lock the console mutex to own the console.
                LG lk(m_mutCons);
                if (!sError.str().empty())
                    std::cout << std::endl << sError.str();
                else
                {
                    const int tPrint = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(time).count());
                    std::cout << std::endl << "Decomposition of " << n << " is impossible. "
                        << "The request took " << tPrint << " ms";
                }
            }
            break;
        }
        case MS::ETypeMes::eAnsYes:
        {
            if (!recvAll(m_idSocket, &c, 1))
            {
                logConnectionLost();
                return;
            }
            const int sz = MS::bufSizeAnsYes(c);
            std::vector<char> buf;
            buf.resize(sz);
            if (!recvAll(m_idSocket, buf.data(), sz))
            {
                logConnectionLost();
                return;
            }
            const std::pair<short, std::vector<number>> ans = MS::deserializeAnsYes(buf);
            const short id = ans.first;
            const std::vector<number>& aComp = ans.second;

            std::ostringstream sError;
            number n = 0;
            TP time = std::chrono::system_clock::now().time_since_epoch();
            {
                LG lk(m_mutTable);
                if (m_table.find(id) != m_table.end())
                {
                    n = m_table[id];
                    time -= m_tableTime[id];
                }
                else
                    sError << "Error: inner table does not contain request id " << id << " received from the server.";
            }

            {
                LG lk(m_mutCons);
                if (!sError.str().empty())
                    std::cout << std::endl << sError.str();
                else
                {
                    const int tPrint = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(time).count());
                    std::cout << std::endl << n << " is decomposable:";
                    for (const number& comp : aComp)
                        std::cout << comp << " ";
                    std::cout << "The request took " << tPrint << " ms";
                }
            }
            break;
        }
        default:
        {
            std::lock_guard<std::mutex> lk(m_mutCons);
            std::cout << std::endl << "Incorrect code from server";
            break;
        }
        }
    }
}

void CRunner::logConnectionLost()
{
    LG lk(m_mutCons);
    std::cout << std::endl << "Connection with server lost";
}