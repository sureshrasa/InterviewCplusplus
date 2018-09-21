/*
 * TestBookAlloc.cpp
 *
 *  Created on: 21 Sep 2018
 *      Author: suresh
 */

#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

int allocate(int const M, vector<int>::const_iterator const & begin,
    vector<int>::const_iterator const & end)
{
    if (M == 1) return accumulate(begin, end, 0);

    int best = numeric_limits<int>::max();

    auto const limit = end - M + 1;
    auto sum = 0;
    for (auto i = begin; i != limit; )
    {
        sum += *i;
        ++i;
        auto bestTail = allocate(M-1, i, end);
        best = min(best, max(sum, bestTail));
    }
    return best;
}

int books(vector<int> &A, unsigned B)
{
    // Do not write main() function.
    // Do not read input, instead use the arguments to the function.
    // Do not print the output, instead return values as specified
    // Still have a doubt. Checkout www.interviewbit.com/pages/sample_codes/ for more details

    if (A.size() < B) return -1;
    if (B == 0) return -1;

    return allocate(B, A.begin(), A.end());
}


