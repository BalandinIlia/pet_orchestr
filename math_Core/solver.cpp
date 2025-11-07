#include "pch.h"
#include "../controls/controls.h"
#include "solver.h"

std::vector<number> solve(number n)
{
	// ans means "answer"
	std::vector<number> ans;
	
	// cnt is the current pow of 3
	number cnt = 1;

	// In this cycle I examine all digits of ternary representation of n from the lowest to the highest.
	// The cycle works the following way:
	// Imagine that n is represented as abcd in the ternary system (where a,b,c,d are digits)
	// 1. At the first step dig=d; after the first step n=abc
	// 2. At the second step dig=c; after the second step n=ab
	// 3. At the third step dig=b; after the third step n=a
	// 4. At the fourth step dig=a; after the fourth step n=0
	// The cycle is finished.
	while (n != 0)
	{
		// dig means "digit"
		const number dig = n % 3;
		if (dig == 2)
		{
			ans.clear();
			Sleep(delayAlgo);
			return ans;
		}
		else if (dig == 1)
			ans.push_back(cnt);
		cnt *= 3;
		n /= 3;
	}
	Sleep(delayAlgo);
	return ans;
}