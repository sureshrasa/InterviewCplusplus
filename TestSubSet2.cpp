/*
 * TestSubSet2.cpp
 *
 *  Created on: 23 Sep 2018
 *      Author: suresh
 */




bool isLess(vector<int> const & a, vector<int> const & b)
{
    return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

bool isPresent(vector<vector<int>> const & r,
    int low,
    int high,
    vector<int> const & s)
{
    while (low <= high)
    {
        auto const mid = (low+high)/2;
        if (isLess(r[mid], s))
        {
            low = mid+1;
        }
        else if (isLess(s, r[mid]))
        {
            high = high-1;
        }
        else
        {
            return true;
        }
    }
    return false;
}

vector<vector<int> > subsetsWithDup(vector<int> &A) {

    vector<vector<int>> result{{}};

    int const size = A.size();
    sort(A.begin(), A.end());

    for (int i = size-1; i >= 0; --i)
    {
        auto const rsize = result.size();
        for (int r = 0; r < rsize; ++r)
        {
            vector<int> s{A[i]};
            s.insert(s.end(), result[r].begin(), result[r].end());

            if (!isPresent(result, 0, rsize-1, s))
            {
                result.push_back(s);
            }
        }
        sort(result.begin(), result.end(), &isLess);
    }
    return result;
}
