/*
 * TestOneEdit.cpp
 *
 *  Created on: 21 Sep 2018
 *      Author: suresh
 */

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <cassert>

using namespace std;

bool isOneEdit(string const & a, string const & b)
{
  if ((a.length() == b.length() && a.length() == 0) ||
      abs(static_cast<int>(a.length()-b.length())) > 1)
    return false;

  auto const aend = a.end();
  auto const bend = b.end();

  auto apos = a.begin();
  auto bpos = b.begin();

  for(; apos != aend && bpos != bend && *apos==*bpos; ++apos, ++bpos);

  auto const alen = std::distance(apos, aend);
  auto const blen = std::distance(bpos, bend);

  if (alen < blen)
    return equal(apos, aend, std::next(bpos));

  if (alen > blen)
    return equal(bpos, bend, std::next(apos));

  return equal(std::next(apos), aend, std::next(bpos));

}

// To execute C++, please define "int main()"
int main() {
  assert(!isOneEdit("", ""));
  assert(!isOneEdit("abc", ""));
  assert(!isOneEdit("", "abc"));
  assert(!isOneEdit("abc", "abcef"));
  assert(!isOneEdit("abcdf", "abc"));

  assert(isOneEdit("", "a"));
  assert(isOneEdit("a", ""));
  assert(isOneEdit("a", "d"));

  assert(isOneEdit("abc", "abd"));
  assert(isOneEdit("abc", "adc"));
  assert(isOneEdit("abc", "dbc"));

  assert(isOneEdit("ab", "abd"));
  assert(isOneEdit("ac", "abc"));
  assert(isOneEdit("bc", "abc"));

  assert(isOneEdit("abc", "ab"));
  assert(isOneEdit("abc", "ac"));
  assert(isOneEdit("abc", "bc"));

  return 0;
}




