/*
 * TestMaths.cpp
 *
 *  Created on: 20 Sep 2018
 *      Author: suresh
 */

#include "TestMaths.h"
#include <cassert>
#include <iostream>

namespace
{
    bool isPower(int const A)
    {
        std::cout << "checking A=" << A << "\n";
        if (A==1) return true;

        int base = 2;
        while (base < A/2)
        {
            std::cout << "checking base=" << base << "\n";
            int acc = A;
            while (acc > 1)
            {
                if (acc % base != 0) break;

                std::cout << "acc=" << acc << "\n";
                acc /= base;
            }

            if (acc == 1)
            {
                std::cout << "found base=" << base << "\n";
                return true;
            }

            ++base;
            std::cout << "trying base=" << base << "\n";
       }
        return false;
    }
}


void suriar::testMaths()
{
    assert(isPower(1024000000));
}


