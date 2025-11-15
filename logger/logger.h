#pragma once
#include "string"
#include "../parameters/controls.h"

// This file contains logger interface. The logger is thread-safe: it can be safely called from 
// different threads concurrently

// Logs a message
void log(const std::string& s, bool bError = false);
void log(const std::string& s1, const std::string& s2, bool bError = false);
void log(const std::string& s1, int v, bool bError = false);
void log(const std::string& s1, number n, bool bError = false);

// Sets name for the current thread
void setThreadName(const std::string& name);