#pragma once
#include "mutex"

// Ip address type
typedef unsigned long IPAddr;
// TCP port type
typedef unsigned short TCPPort;
// number type, - number used in the mathematical problem
typedef unsigned long long number;
// LG = "Lock Guard"
typedef std::lock_guard<std::mutex> LG;

// server ip address
extern const IPAddr addrIP;
// server TCP port
extern const TCPPort port;

// artificial mathematical core algorithm delay
// This delay is necessary to simulate complex core algorithm
extern const int delayAlgo;
