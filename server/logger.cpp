#include "iostream"
#include "mutex"
#include "logger.h"
#include "../controls/controls.h"

// Mutex which makes the function thread-safe
static std::mutex mutCons;

void log(const std::string& s)
{
	LG lk(mutCons);
	std::cout << std::endl << s;
}