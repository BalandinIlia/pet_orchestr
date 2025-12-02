#include "thread"
#include "sstream"
#include "mutex"
#include "functional"
#include "../logger/logger.h"
#include "../networking_library/messages.h"
#include "../networking_library/inner_interaction.h"
#include "../networking_utils/send_receive.h"
#include "../networking_utils/make_socket.h"
#include "clientInstance.h"

/// <summary>
/// This function solves a request from a particular client: 
/// it gets the answer and sends it over TCP.
/// </summary>
/// <param name="idReq"> in. Request id </param>
/// <param name="num"> in. Request body </param>
/// <param name="sock"> in. Socket </param>
/// <param name="mutSocket"> in. 
/// Socket mutex: the mutex which need to be locked before sending data through socket 
/// </param>
/// <param name="idClient"> in. Client id (for logging) </param>
void solveCase(short idReq, number num, const SOCK& sock, std::mutex* mutSocket, int idClient)
{
	CThreadName tn("Case thread");
	{
		std::ostringstream mes;
		mes << "Working with client " << idClient << " starting solving case with number " 
			<< num << " and id " << idReq;
		LOG1(mes.str())
	}

	const std::optional<SOCK> sockService = connectToService();
	if(sockService == std::nullopt)
	{
		LOG2("Failed to connect to service", true)
		return;
	}
	else
		LOG2("Connected to service. Service socket id:", sockService.value())

	std::optional<std::vector<number>> aNum = IC::ask(sockService.value(), num);
	if(aNum == std::nullopt)
	{
		LOG2("Failed to get an answer from cache service", true)
		return;
	}
	else
		LOG2("Received an answer from cache service:", aNum.value())

	// boolean flag saying if everything was sent successfully, or there was a connection error
	bool bSent = true;
	if (aNum.value().empty())
	{
		std::array<char, 3> buf = MS::serializeAnsNo(idReq);
		LG lk(*mutSocket);
		bSent = sendAll(sock, buf.data(), 3);
	}
	else
	{
		std::vector<char> buf = MS::serializeAnsYes(aNum.value(), idReq);
		LG lk(*mutSocket);
		bSent = sendAll(sock, buf.data(), static_cast<int>(buf.size()));
	}

	if(!bSent)
	{
		std::ostringstream mes;
		mes << "Unable to sent an answer to client " << idClient << " for request with id " << idReq << " due to network error";
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
				// buf means "buffer"
				std::array<char, 10> buf;
				if(!recvAll(m_sock, buf.data(), 10))
				{
					LOG2("Unable to get a message from the client", true)
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
	// served socket
	SOCK m_sock;

	// client id
	int m_idClient;

	// mutex for sending data by socket
	// This mutex is necessary because several threads may be sending data simultaneously and
	// this mutex prevents this data from mixturing
	std::mutex m_mutSend;
};

void serveClient(SOCK&& sock, int idClient)
{
	CThreadName tn("Client thread");
	LOG2("Starting serving a client with id", idClient)

	CThreadClient thr(std::move(sock), idClient);
	thr.run();
	LOG1("Finished serving a client with id", idClient)
}