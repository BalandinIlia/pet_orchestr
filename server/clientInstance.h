#pragma once
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "cstring"
#include "iostream"

// This fucntion serves a client with given id connected to given socket
void serveClient(int id, int idClient);