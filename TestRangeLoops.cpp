/*
 *  Created on: 17 Sep 2018
 *      Author: suresh
 */

#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>

#include "TestRangeLoops.h"

void suriar::testRangeLoops()
{
	std::vector<int> const s{1, 2, 3, 4};

	std::cout << "Example 1:" << std::endl;
	for(auto && i : s) { std::cout << i << ","; }
	std::cout << std::endl;

	std::cout << "Example 2:" << std::endl;
	std::copy(std::begin(s), std::end(s), std::ostream_iterator<int>(std::cout, ","));
	std::cout << std::endl;

	std::cout << "Example 3:" << std::endl;
	auto s2 = { 9, 8, 7 };
	std::copy(std::begin(s2), std::end(s2), std::ostream_iterator<int>(std::cout, ","));
	std::cout << std::endl;
}
