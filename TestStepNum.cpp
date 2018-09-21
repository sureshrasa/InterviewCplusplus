/*
 * TestStepNum.cpp
 *
 *  Created on: 21 Sep 2018
 *      Author: suresh
 */

#include "TestStepNum.h"
#include <iterator>
#include <vector>
#include <limits>
#include <cmath>
#include <cassert>

using namespace std;

vector<int> getDigits(int num)
{
    vector<int> result ;
    while (num > 0)
    {
        result.push_back(num%10);
        num /= 10;
    }
    return result;
}

vector<int> genRange(int const digit)
{
    vector<int> result;

    if (digit > 0) result.push_back(digit - 1);
    if (digit < 9) result.push_back(digit + 1);

    return result;
}

vector<int> digitsTo9(int n)
{
    vector<int> result;
    while (n <= 9)
    {
        result.push_back(n);
        ++n;
    }
    return result;
}

vector<int> stepNumRange(bool const isLeadingZero, vector<int> digits, int const radix, int const first, int const limit)
{
    /*
    std::cout << "step radix=" << radix << " ---\n";
    std::copy(digits.begin(), digits.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "---\n";
    */

    if (radix == 0) return digits;

    vector<int> result;
    for ( auto const digit : digits)
    {
        int const digitValue = digit * radix;

        bool const isZero = isLeadingZero && (digit == 0);
        auto const suffixDigits = isZero ? digitsTo9(0) : genRange(digit);
        auto const suffixValues =
            stepNumRange(isZero, suffixDigits,radix/10, -1, std::numeric_limits<int>::max());
        for (int s : suffixValues)
        {
            int const n = digitValue + s;

            if (n > limit) return result;

            if (n >= first) result.push_back(n);
        }
    }
    return result;
}


vector<int> stepnum(int A, int B) {
    // Do not write main() function.
    // Do not read input, instead use the arguments to the function.
    // Do not print the output, instead return values as specified
    // Still have a doubt. Checkout www.interviewbit.com/pages/sample_codes/ for more details

    auto const digits = getDigits(A);
    auto const digitsB = getDigits(B);

    //if (digits.size() < 2) return vector<int>{};

    auto const firstDigits = (digits.size() < digitsB.size())
        ? digitsTo9(0) : digitsTo9(digits[digits.size()-1]);
    return stepNumRange(true, firstDigits, pow(10,digitsB.size()-1), A, B);
 }

void suriar::testStepNum()
{
    assert(stepNum())
}

