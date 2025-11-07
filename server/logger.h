#pragma once
#include "string"

// This function outputs server logs to console. The function is thread-safe, so it
// can be called from different threads.
void log(const std::string& s);