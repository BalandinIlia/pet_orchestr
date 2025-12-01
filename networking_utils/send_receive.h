#pragma once
#include "optional"
#include "make_socket.h"
#include "../parameters/controls.h"

// This is an utility function for receiving bytes from socket. Unlike system recv, it grants 
// that all len bytes are received: it finishes only when all of them are received. It returns 
// true if all bytes were successfully received and false in case of some error.
bool recvAll(const SOCK& id, char* buf, int len);

// This is an utility function for sending bytes to socket. Unlike system send, it grants
// that all len bytes are sent: it finishes only when all of them are sent. It returns true
// if all bytes were successfully sent and false in case of some error.
bool sendAll(const SOCK& id, char* buf, int len);

// This function sends a particular number.
// This function returns true if ok and false in case of error.
bool sendNum(const SOCK& id, number num);

// This function receives a particular number from socket.
// This function returns a number if all is ok and nulopt in case of error.
std::optional<number> recvNum(const SOCK& id);