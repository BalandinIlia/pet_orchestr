#pragma once
#include "string"
#include "vector"
#include "../parameters/controls.h"

// OVERVIEW:
// This file contains logger interface.
// The logger is thread-safe: it can be safely called from different threads concurrently.
// 
// THREAD IDENTIFICATION:
// Each thread is assigned:
//    1. A name: Name is "type" of thread, for example "main thread", which serves the main
//       socket, or "client thread", which serves a particular client.
//    2. A number: The number allows to distinguish between threads with the same name. Name +
//       number is a unique thread identifier.
// 
// LOG MESSAGES:
// There are 2 possible types of log entry: information and error
// Each log entry contains the following fields:
//    1. thread: thread name + thread number
//    2. function: name of the function from which the message was logged
//    3. message: the message itself

// These are functions for message logging. They have the following arguments:
// 1. fn: name of the function from which the message was logged
// 2. s: the message
// 3. optional argument: the second part of the message
// 4. bError: control flag: it tells if this message is of error type or not
void log(const char* fn, const std::string& s, bool bError = false);
void log(const char* fn, const std::string& s, const std::string& s2, bool bError = false);
void log(const char* fn, const std::string& s, int v, bool bError = false);
void log(const char* fn, const std::string& s, number n, bool bError = false);
void log(const char* fn, const std::string& s, const std::vector<number>& v, bool bError = false);

// macroses to simplify calling log functions
// Here are three macroses: for passing 1 argument, 2 arguments and 3 arguments
// The caller doesn't have to pass function name: it is passed automatically through __func__
#define LOG1(a1) {log(__func__, (a1));}
#define LOG2(a1, a2) {log(__func__, (a1), (a2));}
#define LOG3(a1, a2, a3) {log(__func__, (a1), (a2), (a3));}

// Sets name for the current thread
class CThreadName
{
public:
    CThreadName(const std::string& name);

    CThreadName(const CThreadName& inst) = delete;
    CThreadName& operator=(const CThreadName& inst) = delete;
    CThreadName(CThreadName&& inst) = delete;
    CThreadName& operator=(CThreadName&& inst) = delete;

    ~CThreadName();
};