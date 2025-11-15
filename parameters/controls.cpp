#define IN_PARAM 

#include "netinet/in.h"
#include "controls.h"

static_assert(sizeof(IPAddr) == 4, "wrong type for IP address");

static_assert(sizeof(TCPPort) == 2, "wrong type for TCP port");
const TCPPort portInfo = 40000;
const TCPPort portLive = 40001;
const TCPPort portReady = 40002;

static_assert(sizeof(number) == 8, "wrong type of number type");

const int delayAlgo = 5000;
