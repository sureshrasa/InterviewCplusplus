bool operator <(vector<int> const & a, vector<int> const & b)
{
    return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

vector<vector<int> > subsetsWithDup(vector<int> &A) {

    vector<vector<int>> result{{}};

    int const size = A.size();
    sort(A.begin(), A.end());

    int lastIndex = -1;
    for (int i = size-1; i >= 0; --i)
    {
        auto const rsize = result.size();
        auto const start = (i < size-1 && A[i] == A[i+1]) ? lastIndex : 0;
        lastIndex = rsize;
        for (int r = start; r < rsize; ++r)
        {
            vector<int> s{A[i]};
            s.insert(s.end(), result[r].begin(), result[r].end());
            result.push_back(s);
        }
    }

    sort(result.begin(), result.end());
    return result;
}
