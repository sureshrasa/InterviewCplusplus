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

using namespace suriar;

int main()
{
	testVariadicTemplates();
	testRangeLoops();
	testConstructors();
	testGenericCRTP();

	return 0;
}
