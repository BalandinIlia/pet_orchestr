#pragma once
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "cstring"
#include "iostream"

// This is an utility function for receiving bytes from socket. Unlike system recv, it grants that all len
// bytes are received: it finishes only when all of them are received. It returns true if all bytes were
// successfully received and false in case of some error.
bool recvAll(int id, char* buf, int len);

// This is an utility function for sending bytes to socket. Unlike system send, it grants that all len
// bytes are sent: it finishes only when all of them are sent. It returns true if all bytes were
// successfully sent and false in case of some error.
bool sendAll(int id, char* buf, int len);