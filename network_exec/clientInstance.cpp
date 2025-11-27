#include "thread"
#include "sstream"
#include "mutex"
#include "functional"
#include "../logger/logger.h"
#include "clientInstance.h"
#include "../networking_library/messages.h"
#include "../networking_library/inner_interaction.h"
#include "../networking_utils/send_receive.h"
#include "../networking_utils/make_socket.h"

/// <summary>
/// This function solves a request from a particular client: it calculates the answer and sends it over TCP.
/// </summary>
/// <param name="id"> in. Request id </param>
/// <param name="num"> in. Request body </param>
/// <param name="idSocket"> in. Socket id </param>
/// <param name="mutSocket"> in. Socket mutex: the mutex which need to be locked before sending data through socket </param>
/// <param name="idClient"> in. Client id (for logging) </param>
void solveCase(short id, number num, const SOCK& idSocket, std::mutex* mutSocket, int idClient)
{
	setThreadName("Case thread");
	LOG2("Starting solving a case for number", num)

	SOCK idSocketService = connectToService();
	LOG2("Service socket id", idSocketService)

	std::optional<std::vector<number>> aNum = askInner(idSocketService, num);
	if(aNum == std::nullopt)
	{
		LOG2("Failed to get an answer from cache service", true)
		return;
	}

	// boolean flag saying if everything was sent successfully, or there was a connection error
	bool bSent = true;
	if (aNum.value().empty())
	{
		std::array<char, 3> buf = MS::serializeAnsNo(id);
		LG lk(*mutSocket);
		bSent = sendAll(idSocket, buf.data(), 3);
	}
	else
	{
		std::vector<char> buf = MS::serializeAnsYes(aNum.value(), id);
		LG lk(*mutSocket);
		bSent = sendAll(idSocket, buf.data(), static_cast<int>(buf.size()));
	}

	if(!bSent)
	{
		std::ostringstream mes;
		mes << "Unable to sent an answer to client " << idClient << " for request with id " << id << " due to network error";
		LOG2(mes.str(), true)
	}
}

/// <summary>
/// This class runs the thread serving particular client
/// </summary>
class CThreadClient
{
public:
	CThreadClient(SOCK&& sock, int idClient) : 
		m_sock(std::move(sock)),
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
			if (!recvAll(m_sock, &c, 1))
			{
				// log connection error and stop execution
				LOG2("Session closed due to network error", true)
				break;
			}
			if (MS::decodeType(c) != MS::ETypeMes::eReq)
			{
				LOG2("Client sent a message with incorrect code", true)
				break;
			}
			else
			{
				std::array<char, 10> buf;
				if(!recvAll(m_sock, buf.data(), 10))
				{
					LOG2("Client sent a broken message", true)
					break;
				}

				std::pair<short, number> req = MS::deserializeRequest(buf);
				
				std::ostringstream mes;
				mes << "Client sent a request with id " << req.first << " and number " << req.second;
				LOG1(mes.str())

				// Each request is processed in a separate thread. This allows processing several requests from the same
				// client simultaneously.
				std::thread t(solveCase, req.first, req.second, std::cref<SOCK>(m_sock), &m_mutSend, m_idClient);
				aThrChild.push_back(std::move(t));
			}
		}
		// I come here only in case of an error: connection error, or incorrect message code from the client.
		// I wait for all child processes to finish, since they have links/pointers to objects from the class.
		for (std::thread& t : aThrChild)
			t.join();
	}

private:
	SOCK m_sock;

	int m_idClient;

	// mutex for sending data by socket
	// This mutex is necessary because several threads may be sending data simultaneously and this mutex prevents
	// this data from mixturing
	std::mutex m_mutSend;
};

void serveClient(SOCK&& sock, int idClient)
{
	setThreadName("Client thread");
	LOG1("Starting serving a client")

	CThreadClient thr(std::move(sock), idClient);
	thr.run();
	LOG1("Finished serving a client")
}
