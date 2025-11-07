#include "pch.h"
#include "winsock2.h"
#include "controls.h"

static_assert(sizeof(IPAddr) == 4, "wrong type for IP address");
const IPAddr addrIP = INADDR_LOOPBACK;

static_assert(sizeof(TCPPort) == 2, "wrong type for TCP port");
const TCPPort port = 5555;

static_assert(sizeof(number) == 8, "wrong type of number type");

const int delayAlgo = 5000;