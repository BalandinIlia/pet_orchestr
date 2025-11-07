#pragma once
#include "winsock2.h"

// This is an utility class: it configures environment for windows sockets when it is constructed and
// deconfigures it when it is destructured.
// The purpose of the class is the following: the caller creates an instance of the class when they start
// working with sockets. When the function finishes, instance of the class is destructed automatically and
// there is no risk to forget to deconfigure sockets.
class CWSAConfig
{
public:
    CWSAConfig();
    ~CWSAConfig();
};

// This is an utility function for receiving bytes from socket. Unlike system recv, it grants that all len
// bytes are received: it finishes only when all of them are received. It returns true if all bytes were
// successfully received and false in case of some error.
bool recvAll(SOCKET id, char* buf, int len);

// This is an utility function for sending bytes to socket. Unlike system send, it grants that all len
// bytes are sent: it finishes only when all of them are sent. It returns true if all bytes were
// successfully sent and false in case of some error.
bool sendAll(SOCKET id, char* buf, int len);