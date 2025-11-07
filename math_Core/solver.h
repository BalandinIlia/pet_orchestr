#pragma once
#include "../controls/controls.h"

/// <summary>
/// Core mathematical algorithm, which solves the problem: can given numer be decomposed into sum of different pows of 3.
/// For example 82 can be decomposed: 82=3^4+3^0.
/// But 6 can't be. However 6=3^1+3^1, but 1=1, so pows are equal.
/// 
/// The core idea of the algorithm is the following:
/// Every number has a unique presentation in the ternary number system. Let's say some number X has 3-digit presentation:
/// X=abc. It means that X=a*3^2+b*3^1+c*3^0. This is the unique presentation of X as a sum of pows of 3 with coefficients
/// (a,b,c) which can be either 0 or 1 or 2. X can be represented as a sum of different pows of 3 if all coefficients are 0 
/// or 1. I use this fact to decompose X (or find out that it can't be decomposed into different pows of 3): I calculate
/// coefficients of its ternary representation.
/// </summary>
/// <param name="n"> The number </param>
/// <returns> the answer </returns>
std::vector<number> solve(number n);