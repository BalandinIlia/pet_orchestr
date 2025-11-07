#include "pch.h"
#include "host.h"
#include "solver.h"

std::unique_ptr<CMathCoreHost> CMathCoreHost::pOne;

std::mutex CMathCoreHost::m_mutCreate;

CMathCoreHost::CMathCoreHost() {}

CMathCoreHost& CMathCoreHost::one()
{
	LG lk(m_mutCreate);
	if (pOne.get() == nullptr)
	{
		pOne = std::move(std::unique_ptr<CMathCoreHost>(new CMathCoreHost));
	}
	return *pOne;
}

std::vector<number> CMathCoreHost::get(number num)
{
	{
		// check if the answer is already cached
		LG lk(m_mutMap);
		if (m_answers.find(num) != m_answers.end())
		{
			const auto& ret = m_answers[num];
			return ret;
		}
	}

	const auto ans = solve(num);
	
	LG lk(m_mutMap);
	// Here I do the second check if the answer is already cached.
	// This check is neccessary even after the first check, because 
	// the answer could have been added to the cache after the first
	// check in another thread while this thread was running the 
	// mathematical algorithm.
	if (m_answers.find(num) != m_answers.end())
	{
		const auto& ret = m_answers[num];
		return ret;
	}
	m_answers[num] = ans;
	return ans;
}

CMathCoreHost::~CMathCoreHost()
{}