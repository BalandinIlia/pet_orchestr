#pragma once
#include "optional"
#include "../parameters/controls.h"

// This file contains utilities for creating TCP sockets.

// Special class for creating SOCK instances
class CSOCKFactory;

// Instance of this class owns a TCP socket.
// The class makes sure that the socket is closed after it is not used anymore.
// Class is named "SOCK". This is an abbreviation from "socket".
class SOCK
{
private:
    SOCK();

public:
    // Copying must be prohibited, since copying can create several SOCK instances owning
    // the same TCP socket.
    // If there are several SOCK instances owning the same socket, this socket will be closed
    // several times.
    SOCK(const SOCK& inst) = delete;
    SOCK& operator=(const SOCK& inst) = delete;

    // functions for moving SOCK instance
    SOCK(SOCK&& inst);
    SOCK& operator=(SOCK&& inst);
    SOCK mv() { return SOCK(std::move(*this)); }

    // Function for listening socket: this function waits for incoming connection and returns
    // a connected socket representing the established connection
    SOCK acceptS() const;

    operator SOCKET() const { return m_id; }

    // Destructor. This function closes the owned socket.
    ~SOCK();

private:
    // Socket identified in the operating system
    SOCKET m_id;

friend class CSOCKFactory;
};

// This function creates a listening socket. The socket listens for incoming work connections.
// If socket creation fails, std::nullopt is returned.
std::optional<SOCK> listenInfo();

// This class manages interaction with kubernetes: it manages response to kubernetes liveness probe.
class CInteractKuberentes
{
public:
    // Start responding to kubernetes liveness probe.
    static void start();
    // Terminate responding to kubernetes liveness probe. Once done this function can't be undone.
    static void terminateLive();

private:
    // This function responds to kuberentes liveness probe
    static void informLive();

private:
    // control flag: the class reponds to kubernetes liveness probe only if it is true
    static bool m_live;

    // mutex protecting the control flag
    static std::mutex m_mutLive;
};

// Connects to a service pod within kubernetes cluster. Returns the connection socket or nullopt
// if the connection failed.
std::optional<SOCK> connectToService();