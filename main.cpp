/*
 * main.cpp
 *
 *  Created on: 17 Sep 2018
 *      Author: suresh
 */

#include "TestVariadicTemplates.h"
#include "TestRangeLoops.h"
#include "TestConstructors.h"
#include "TestGenericCRTP.h"
#include "TestMove.h"
#include "TestIterators.h"

using namespace suriar;

int main()
{
	testVariadicTemplates();
	testRangeLoops();
	testConstructors();
	testGenericCRTP();
	testMove();
	testIterators();

	return 0;
}
