#include "thread"
#include "sstream"
#include "mutex"
#include "../logger/logger.h"
#include "clientInstance.h"
#include "../networking_library/messages.h"
#include "../networking_utils/send_receive.h"
#include "../math_core/host.h"

/// <summary>
/// This function solves a request from a particular client: it calculates the answer and sends it over TCP.
/// </summary>
/// <param name="id"> in. Request id </param>
/// <param name="num"> in. Request body </param>
/// <param name="idSocket"> in. Socket id </param>
/// <param name="mutSocket"> in. Socket mutex: the mutex which need to be locked before sending data through socket </param>
/// <param name="idClient"> in. Client id (for logging) </param>
void solveCase(short id, number num, int idSocket, std::mutex* mutSocket, int idClient)
{
	setThreadName("Case thread");
	std::ostringstream mes;
	mes << "Starting solving a case for number" << num;
	log(mes.str());

	std::vector<number> aNum;
	aNum.push_back(2);
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
		log("Couldn't sent the answer", true);
	else
		log("Answer sent successfully, finishing the thread");
}

/// <summary>
/// This class runs the thread serving particular client
/// </summary>
class CThreadClient
{
public:
	CThreadClient(int idSocket, int idClient) : 
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
				log("Session closed due to network error", true);
				break;
			}
			if (MS::decodeType(c) != MS::ETypeMes::eReq)
			{
				log("Client sent a message with incorrect code", true);
				break;
			}
			else
			{
				std::array<char, 10> buf;
				if(!recvAll(m_idSocket, buf.data(), 10))
				{
					log("Client sent a broken message", true);
					break;
				}

				std::pair<short, number> req = MS::deserializeRequest(buf);
				
				std::ostringstream mes;
				mes << "Client sent a request with id " << req.first << " and number " << req.second;
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
	int m_idSocket;

	int m_idClient;

	// mutex for sending data by socket
	// This mutex is necessary because several threads may be sending data simultaneously and this mutex prevents
	// this data from mixturing
	std::mutex m_mutSend;
};

void serveClient(int idSocket, int idClient)
{
	setThreadName("Client thread");
	log("Starting serving a client");

	CThreadClient thr(idSocket, idClient);
	thr.run();
	log("Finished serving a client, closing socket");
	close(idSocket);
}
