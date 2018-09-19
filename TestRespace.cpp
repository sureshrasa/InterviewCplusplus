/*
 * TestRespace.cpp
 *
 *  Created on: 19 Sep 2018
 *      Author: suresh
 */

#include "TestRespace.h"
#include "Trie.hpp"
#include "boost/range/iterator_range_core.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <iterator>
#include <algorithm>
#include <cassert>

using namespace suriar;

namespace
{
    typedef Trie<std::string, std::string> Dict;
    typedef HashTrie<std::string, std::string> HashDict;

    class Respacer
    {
    public:
        std::vector<std::string> respace(std::string const & str, Dict const & dict)
        {
            auto str_end = std::end(str);
            auto best = respace(std::begin(str), str_end, dict);
            
            return buildWords(best_respace(best.first, std::begin(best.second), std::end(best.second), str_end, dict).second);
        }

    private:
        typedef boost::iterator_range<std::string::const_iterator> iterator_range;
        typedef std::pair<bool, iterator_range> word_match;
        typedef std::vector<word_match> match_vector;

        std::map<unsigned, std::pair<unsigned, match_vector>> cache;

        std::vector<std::string> buildWords(match_vector const & word_matches)
        {
            std::vector<std::string> result;

            std::transform(word_matches.begin(), word_matches.end(),
                    std::back_inserter(result),
                    [](auto const & s){return std::string(s.second.begin(), s.second.end());});

            return result;
        }

        std::pair<unsigned, match_vector>
        best_respace(unsigned const badCharLimit,
                match_vector::const_iterator const & begin,
                match_vector::const_iterator const & end,
                std::string::const_iterator const & strEnd, Dict const & dict)
        {
            if (begin == end) return std::make_pair(0, match_vector{});
            
            auto const & firstWord = *begin;
            if (!firstWord.first)
            {
                auto const badChars = std::distance(std::begin(firstWord.second), std::end(firstWord.second));
                if (badChars > badCharLimit)
                    return std::make_pair(std::numeric_limits<unsigned>::max(), match_vector{});
                
                auto const result = best_respace(badCharLimit-badChars, std::next(begin), end, strEnd, dict);
                return std::make_pair(badChars+result.first, merge_words(firstWord, result.second));
            }
            else
            {
                auto const result1 = best_respace_good(badCharLimit, firstWord, strEnd, dict);
                auto const result2 = best_respace(badCharLimit, std::next(begin), end, strEnd, dict);
                if (result1.first <= result2.first)
                {
                    return result1;
                }
                else
                {
                    return std::make_pair(result2.first, concat(firstWord, result2.second));
                }
            }
        }

        std::pair<unsigned, match_vector>
        best_respace_good(unsigned const badCharLimit,
                word_match const & goodWord,
                std::string::const_iterator const & strEnd, Dict const & dict)
        {
            auto const range = goodWord.second;
            unsigned badChars = 0;
            auto bestSoFar = std::make_pair(std::numeric_limits<unsigned>::max(), match_vector{});
            for (auto i = range.begin(); i != range.end();)
            {
                ++i;
                ++badChars;
                if (badChars < badCharLimit)
                {
                    auto result = respace(i, strEnd, dict);
                    if (result.first+badChars < bestSoFar.first)
                    {
                        auto const badWord = std::make_pair(false, boost::make_iterator_range(range.begin(), i));
                        bestSoFar = std::make_pair(badChars+result.first, merge_words(badWord, result.second));
                    }
                }
                else
                    break;
            }
            return bestSoFar;
        }

        match_vector merge_words(word_match const & badWord, match_vector const & words)
        {
            if (words.empty()) return match_vector{badWord};
            
            auto const & firstWord = words[0];
            if (!firstWord.first)
            {
                auto result = match_vector{mergeAdjacentBadWordPair(badWord, firstWord)};
                result.insert(result.end(), std::next(words.begin()), words.end());
                return result;
            }
            else
            {
                return concat(badWord, words);
            }
        }
        
        match_vector concat(word_match const & word, match_vector const & words)
        {
            auto result = match_vector{word};
            result.insert(result.end(), words.begin(), words.end());
            return result;
        }

        word_match mergeAdjacentBadWordPair(word_match const & before, word_match const & after)
        {
            return std::make_pair(false, boost::make_iterator_range(std::begin(before.second), std::end(after.second)));
        }
        
        std::pair<unsigned, match_vector>
        respace(std::string::const_iterator const & begin, std::string::const_iterator const & end, Dict const & dict)
        {
            auto const searchPos = std::distance(begin, end);
            auto const pos = cache.find(searchPos);
            if (pos != cache.end()) return pos->second;

            unsigned badCount = 0;
            match_vector words;

            auto bad = boost::make_iterator_range(begin, begin);
            for(;;)
            {
                std::cout << "respacing: <" << std::string(std::end(bad), end) << ">\n";
                auto match = dict.longest_prefix_path(boost::make_iterator_range(std::end(bad), end));
                while (match.second)
                {
                    badCount += addIfNonEmpty(words, bad);
                    std::cout << "found good word: <" << *match.second << ">\n";
                    words.push_back(std::make_pair(true, match.first));
                    bad = boost::make_iterator_range(std::end(match.first), std::end(match.first));
                    match = dict.longest_prefix_path(boost::make_iterator_range(std::end(bad), end));
                }
                if (std::end(bad) == end)
                    break;

                bad = boost::make_iterator_range(std::begin(bad), std::next(std::end(bad)));
            }
            badCount += addIfNonEmpty(words, bad);

            auto const result = std::make_pair(badCount, words);
            cache[searchPos] = result;
            return result;
        }

        template <typename T>
        static unsigned addIfNonEmpty(match_vector & result, T const & range)
        {
            if (std::begin(range) != std::end(range))
            {
                std::cout << "found bad word: <" << std::string(std::begin(range), std::end(range)) << ">\n";
                result.push_back(std::make_pair(false, range));
                return std::distance(std::begin(range), std::end(range));
            }
            return 0;
        }
    };

    void addWord(Dict & dict, std::string const & word)
    {
        dict.put(word, word);
    }
}

void suriar::testRespace()
{
    auto respacer = std::make_shared<Respacer>();

    auto const dict = std::make_shared<HashDict>();
    addWord(*dict, "looked");
    addWord(*dict, "just");
    addWord(*dict, "like");
    addWord(*dict, "her");
    addWord(*dict, "brother");
    addWord(*dict, "good");
    addWord(*dict, "bye");
    addWord(*dict, "goodbye");

    assert((respacer->respace("jesslookedjustliketomherbrother", *dict) ==
            std::vector<std::string>{"jess", "looked", "just", "like", "tom", "her", "brother"}));

    assert((respacer->respace("goodbrothertomgoodbyebye", *dict) ==
            std::vector<std::string>{"good", "brother", "tom", "goodbye", "bye"}));
}


