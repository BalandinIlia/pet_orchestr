#pragma once
#include "optional"
#include "../parameters/controls.h"

class CSOCKFactory;

// This class owns a TCP socket.
// The class make sure that the socket is closed after it is now used anymore.
class SOCK
{
private:
    SOCK();

public:
    SOCK(const SOCK& inst) = delete;
    SOCK& operator=(const SOCK& inst) = delete;

    SOCK(SOCK&& inst);
    SOCK& operator=(SOCK&& inst);

    SOCK acceptS(const SOCK& s) const;

    operator SOCKET() const { return m_id; }

    ~SOCK();

private:
    SOCKET m_id;

friend class CSOCKFactory;
};

// returns socket listening to incoming work connections
std::optional<SOCK> listenInfo();

SOCK sockClient();

// class managing Kubernetes interaction: readiness, liveliness, startup
class CInteractKuberentes
{
public:
    static void start();
    static void terminateLive();

private:
    static void informLive();

private:
    static bool m_live;
    static std::mutex m_mutLive;
};

SOCK connectToService();