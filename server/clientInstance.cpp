#include "thread"
#include "sstream"
#include "mutex"
#include "logger.h"
#include "clientInstance.h"
#include "../networking/messages.h"
#include "../networking/utils.h"
#include "../math_Core/host.h"

/// <summary>
/// This function solves a request from a particular client: it calculates the answer and sends it over TCP.
/// </summary>
/// <param name="id"> in. Request id </param>
/// <param name="num"> in. Request body </param>
/// <param name="idSocket"> in. Socket id </param>
/// <param name="mutSocket"> in. Socket mutex: the mutex which need to be locked before sending data through socket </param>
/// <param name="idClient"> in. Client id (for logging) </param>
void solveCase(short id, number num, SOCKET idSocket, std::mutex* mutSocket, int idClient)
{
	std::vector<number> aNum = CMathCoreHost::one().get(num);
	// boolean flag saying if everything was sent successfully, or there was a connection error
	bool bSent = true;
	if (aNum.empty())
	{
		std::array<char, 3> buf = MS::serializeAnsNo(id);
		mutSocket->lock();
		bSent = sendAll(idSocket, buf.data(), 3);
		mutSocket->unlock();
	}
	else
	{
		std::vector<char> buf = MS::serializeAnsYes(aNum, id);
		mutSocket->lock();
		bSent = sendAll(idSocket, buf.data(), static_cast<int>(buf.size()));
		mutSocket->unlock();
	}
	if(!bSent)
	{
		std::ostringstream mes;
		mes << "Unable to sent an answer to client " << idClient << " for request with id " << id << " due to network error";
		log(mes.str());
	}
}

/// <summary>
/// This class runs the thread serving particular client
/// </summary>
class CThreadClient
{
public:
	CThreadClient(SOCKET idSocket, int idClient) : 
		m_idSocket(idSocket),
		m_idClient(idClient)
	{}

	void run()
	{
		// Child processes
		std::vector<std::thread> aThrChild;
		for (;;)
		{
			// request code
			char c;
			if (!recvAll(m_idSocket, &c, 1))
			{
				// log connection error and stop execution
				std::ostringstream mes;
				mes << "Session with client " << m_idClient << " is closed due to network error";
				log(mes.str());
				break;
			}
			if (MS::decodeType(c) != MS::ETypeMes::eReq)
			{
				// // log incorrect code and stop execution
				std::ostringstream mes;
				mes << "Client " << m_idClient << " sent a message with incorrect code";
				log(mes.str());
				break;
			}
			else
			{
				std::array<char, 10> buf;
				if(!recvAll(m_idSocket, buf.data(), 10))
				{
					// log connection error and stop execution
					std::ostringstream mes;
					mes << "Session with client " << m_idClient << " is closed due to network error";
					log(mes.str());
					break;
				}

				std::pair<short, number> req = MS::deserializeRequest(buf);
				
				std::ostringstream mes;
				mes << "Client " << m_idClient << " sent a request with id " << req.first << " and number " << req.second;
				log(mes.str());

				// Each request is processed in a separate thread. This allows processing several requests from the same
				// client simultaneously.
				std::thread t(solveCase, req.first, req.second, m_idSocket, &m_mutSend, m_idClient);
				aThrChild.push_back(std::move(t));
			}
		}
		// I come here only in case of an error: connection error, or incorrect message code from the client.
		// I wait for all child processes to finish, since they have links/pointers to objects from the class.
		for (std::thread& t : aThrChild)
			t.join();
	}

private:
	SOCKET m_idSocket;

	int m_idClient;

	// mutex for sending data by socket
	// This mutex is necessary because several threads may be sending data simultaneously and this mutex prevents
	// this data from mixturing
	std::mutex m_mutSend;
};

void serveClient(SOCKET idSocket, int idClient)
{
	{
		const std::thread::id idThread = std::this_thread::get_id();
		std::ostringstream mes;
		mes << "Thread with id " << idThread << " started serving a client " << idClient << " at socket " << idSocket;
		log(mes.str());
	}

	CThreadClient thr(idSocket, idClient);
	thr.run();
	closesocket(idSocket);
}
