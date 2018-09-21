/*
 * TestLongDiv.cpp
 *
 *  Created on: 21 Sep 2018
 *      Author: suresh
 */




#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int longdiv(vector<int> & N)
{
    int prevCarry = 0;

    auto const end = N.end();
    for (auto i = N.begin(); i != end; ++i)
    {
        auto sum = prevCarry * 10 + *i;
        auto digit = sum/2;
        prevCarry = sum%2;
        *i = digit;
    }
    auto start = N.begin();
    while (start != end & *start == 0) ++start;
    N.erase(N.begin(), start);
    return prevCarry;
}

vector<int> toDigits(string const & A)
{
    vector<int> r;
    transform(A.begin(),A.end(),back_inserter(r),
        [=](char const c)->int{return c-'0';});
    return r;
}


void printN(vector<int> const & N)
{
    cout << "N = " << N.size() << ": ";
    std::copy(N.begin(),N.end(), ostream_iterator<int>(cout, ""));
    cout << "\n";
}

int ispower(string A) {
    // Do not write main() function.
    // Do not read input, instead use the arguments to the function.
    // Do not print the output, instead return values as specified
    // Still have a doubt. Checkout www.interviewbit.com/pages/sample_codes/ for more details

    if (A.empty() || A=="0" || A=="1") return 0;

    auto N = toDigits(A);
    int carry = 0;
    //printN(N);
    while (N.size() >= 2 ||
        (N.size() == 1 && N[0] > 1))
    {
        if (carry != 0) return 0;
        carry = longdiv(N);
        //cout << "after div "; printN(N);
    }
    return 1;
}
