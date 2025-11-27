#pragma once
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "cstring"
#include "iostream"
#include "../networking_utils/make_socket.h"

// This function serves a client with given id connected to given socket
void serveClient(SOCK&& sock, int idClient);