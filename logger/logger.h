#pragma once
#include "string"
#include "vector"
#include "../parameters/controls.h"

// This file contains logger interface. The logger is thread-safe: it can be safely called from 
// different threads concurrently

// Logs a message
void log(const char* fn, const std::string& s, bool bError = false);
void log(const char* fn, const std::string& s1, const std::string& s2, bool bError = false);
void log(const char* fn, const std::string& s1, int v, bool bError = false);
void log(const char* fn, const std::string& s1, number n, bool bError = false);
void log(const char* fn, const std::string& s1, const std::vector<number>& v, bool bError = false);

#define LOG1(a1) {log(__func__, (a1));}
#define LOG2(a1, a2) {log(__func__, (a1), (a2));}
#define LOG3(a1, a2, a3) {log(__func__, (a1), (a2), (a3));}

// Sets name for the current thread
void setThreadName(const std::string& name);