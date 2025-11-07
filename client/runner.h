#pragma once
#include "winsock2.h"
#include "iostream"
#include "thread"
#include "mutex"
#include "map"
#include "chrono"
#include "../networking/messages.h"
#include "../networking/utils.h"

// TP means "Time Point"
typedef std::chrono::time_point<std::chrono::system_clock>::duration TP;

// This class runs interaction with the server
class CRunner
{
public:
	// Create a runner on given (already established) socket
	CRunner(SOCKET idS);

	// Main functions: it runs interaction with the server
	void run();

private:
	// This function takes data from the user and sends it to the server
	void send();
	// This function receives data from the server and displays it to the user
	void receive();

	// These 2 functions are adapters allowing to run send and receive in concurrent threads
	static void sendAdapter(CRunner* r) { r->send(); }
	static void recvAdapter(CRunner* r) { r->receive(); }

	void logConnectionLost();

private:
	// socket id
	const SOCKET m_idSocket;

	// A table with key, - request id and value, - request value
	std::map<short, number> m_table;

	// A table with key, - request id; value, - the time point when the request was sent
	std::map<short, TP> m_tableTime;

	// id of the next request
	short m_id;

	// This is a mutex guarding the tables. It prevents send and receive threads from accessing the tables simultaneously.
	// Now there is only one CRunner instance, so there is no difference if this mutex is static or not.
	// However, if in the future, I decide to create several different runners, each will have its own table, so
	// each should have its own table mutex. So I don't make this mutex static.
	std::mutex m_mutTable;

	// This is a mutex guarding the console. It prevents receive thread from outputting to console while receive thread
	// is taking data from the user.
	// Now there is only one CRunner instance, so there is no difference if this mutex is static or not.
	// However, if in the future, I decide to create several different runners, all of them will have shared console window.
	// So, I make this mutex static, so it is common for all runners. So, console would be owned by only one thread of only
	// one runner at each point in time.
	static std::mutex m_mutCons;

	// I follow the rule: I never lock both mutexes simultaneously in one thread to prevent deadlocks.
};