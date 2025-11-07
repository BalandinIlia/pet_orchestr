#pragma once
#include "winsock2.h"

// This fucntion serves a client with given id connected to given socket
void serveClient(SOCKET id, int idClient);