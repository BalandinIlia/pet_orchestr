#pragma once
#include "map"
#include "mutex"
#include "../controls/controls.h"

/// <summary>
/// This is a host, which keep manages the math part: all problem instances, all solutions and their caching.
/// The host is a singleton to keep everything in one place.
/// The host is thread-safe, - it can be safely called from different threads.
/// </summary>
class CMathCoreHost
{
private:
	CMathCoreHost();

	// the only instance of the host
	static std::unique_ptr<CMathCoreHost> pOne;
public:
	static CMathCoreHost& one();

	/// <summary>
	/// Get the answer for the given number
	/// </summary>
	/// <param name="num"> in. The given number. </param>
	/// <returns> the answer </returns>
	std::vector<number> get(number num);

	~CMathCoreHost();

private:
	// cached answers
	std::map<number, std::vector<number>> m_answers;

	// Mutex protecting the map from simultaneous access from different threads
	std::mutex m_mutMap;

	// Mutex protecting host creation section from simultaneous access from different threads
	static std::mutex m_mutCreate;
};