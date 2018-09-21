/*
 * TestUniquePrefix.cpp
 *
 *  Created on: 21 Sep 2018
 *      Author: suresh
 */

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>

using namespace std;

bool isPrefixOfAny(string const & word,
                      vector<string>::const_iterator const wordPos,
                 vector<string>::const_iterator const begin,
                 vector<string>::const_iterator const end)
{
  auto const len = word.length();

  return any_of(begin, end,
               [=](string const & s)->bool{
                 return &s != &*wordPos && s.substr(0,len)==word;});
  /*
  for (auto w = begin; w != end; ++w)
  {
    if (w != wordPos)
    {
      if ((*w).substr(0, len) == word)
        return true;
    }
  }
  return false;
  */
}

string findPrefix(vector<string>::const_iterator const wordPos,
                 vector<string>::const_iterator const begin,
                 vector<string>::const_iterator const end)
{
  string prefix;
  auto const word = *wordPos;

  std::cout << "findPrefix(" << word << ")\n";
  auto const wordBegin = word.begin();
  auto const wordEnd = word.end();
  auto w = wordBegin;
  do
  {
    ++w;
    auto const prefix = string(wordBegin, w);
    std::cout << "checking prefix<" << prefix << ">\n";
    if (!isPrefixOfAny(prefix, wordPos, begin, end))
    {
      std::cout << "found prfix: <" << prefix <<">\n";
      return prefix;
    }
  } while (w != wordEnd);
  return {};
}

vector<string> shortestPrefix(vector<string> const & words)
{
  vector<string> result;

  auto const end = words.end();
  auto const begin = words.begin();
  for(auto i = begin; i != end; ++i)
  {
    result.push_back(findPrefix(i, begin, end));
  }
  return result;
}

// To execute C++, please define "int main()"
int main() {

  assert((shortestPrefix(vector<string>{"zebra", "dog", "duck", "dove"})
    == vector<string>{"z", "dog", "du", "dov"}));

  return 0;
}


