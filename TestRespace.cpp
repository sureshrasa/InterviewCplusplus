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
        explicit Respacer(std::shared_ptr<Dict const> const & dict): dict{dict} {}

        std::vector<std::string> respace(std::string const & str)
        {
            auto str_end = std::end(str);
            auto best = respace(std::begin(str), str_end);
            
            {
                auto const firstPass = buildWords(best.second);
                std::cout << "first pass [";
                std::copy(firstPass.begin(), firstPass.end(), std::ostream_iterator<std::string>(std::cout, " "));
                std::cout << "]\n";
            }

            auto const result = buildWords(best_respace(best.first, std::begin(best.second), std::end(best.second), str_end).second);
            {
                std::cout << "last pass [";
                std::copy(result.begin(), result.end(), std::ostream_iterator<std::string>(std::cout, " "));
                std::cout << "]\n";
            }

            cache.clear();

            return result;
        }

    private:
        typedef boost::iterator_range<std::string::const_iterator> iterator_range;
        typedef std::pair<bool, iterator_range> word_match;
        typedef std::vector<word_match> match_vector;

        std::shared_ptr<Dict const> const dict;
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
                std::string::const_iterator const & strEnd)
        {
            if (begin == end) return std::make_pair(0, match_vector{});
            
            auto const & firstWord = *begin;
            if (!firstWord.first)
            {
                auto const badChars = std::distance(std::begin(firstWord.second), std::end(firstWord.second));
                if (badChars > badCharLimit)
                    return std::make_pair(std::numeric_limits<unsigned>::max(), match_vector{});
                
                auto const result = best_respace(badCharLimit-badChars, std::next(begin), end, strEnd);
                return std::make_pair(badChars+result.first, merge_words(firstWord, result.second));
            }
            else
            {
                auto const result1 = best_respace_good(badCharLimit, firstWord, strEnd);
                auto const result2 = best_respace(badCharLimit, std::next(begin), end, strEnd);
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
                std::string::const_iterator const & strEnd)
        {
            auto const range = goodWord.second;
            unsigned badChars = 1;
            auto bestSoFar = std::make_pair(std::numeric_limits<unsigned>::max(), match_vector{});
            for (auto i = std::next(range.begin()); i != range.end() && badChars < badCharLimit; ++i, ++badChars)
            {
                auto result = respace(i, strEnd);
                if (result.first+badChars < bestSoFar.first)
                {
                    auto const badWord = std::make_pair(false, boost::make_iterator_range(range.begin(), i));
                    bestSoFar = std::make_pair(badChars+result.first, merge_words(badWord, result.second));
                }
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
        
        std::string buildWord(word_match const & match)
        {
            return std::string(match.second.begin(), match.second.end());
        }

        std::pair<unsigned, match_vector>
        respace(std::string::const_iterator const & begin, std::string::const_iterator const & end)
        {
            std::cout << "respacing <" << std::string(begin, end) << ">\n";

            if (begin == end) return std::make_pair(0, match_vector{});

            auto const searchPos = std::distance(begin, end);
            auto const pos = cache.find(searchPos);
            if (pos != cache.end()) return pos->second;

            auto match = dict->longest_prefix_path(boost::make_iterator_range(begin, end));

            if (match.second)
            {
                std::cout << "found word<" << *match.second << ">\n";
                auto const match_word = std::make_pair(true, match.first);
                auto const rest = respace(match.first.end(), end);
                auto const result = std::make_pair(rest.first, concat(match_word, rest.second));
                cache[searchPos] = result;
                return result;
            }
            else
            {
                auto const badWord = std::make_pair(false, boost::make_iterator_range(begin, std::next(begin)));
                auto const rest = respace(badWord.second.end(), end);
                auto const result = std::make_pair(rest.first+1, merge_words(badWord, rest.second));
                std::cout << "found bad word<" << buildWord(result.second[0]) << ">\n";
                cache[searchPos] = result;
                return result;
            }
        }
    };

    void addWord(Dict & dict, std::string const & word)
    {
        dict.put(word, word);
    }
}

void suriar::testRespace()
{
    auto const dict = std::make_shared<HashDict>();
    addWord(*dict, "looked");
    addWord(*dict, "just");
    addWord(*dict, "like");
    addWord(*dict, "her");
    addWord(*dict, "brother");
    addWord(*dict, "good");
    addWord(*dict, "bye");
    addWord(*dict, "goodbye");

    auto respacer = std::make_shared<Respacer>(dict);

    assert((respacer->respace("jesslookedjustliketomherbrother") ==
            std::vector<std::string>{"jess", "looked", "just", "like", "tom", "her", "brother"}));

    assert((respacer->respace("goodbrothertomgoodbyebye") ==
            std::vector<std::string>{"good", "brother", "tom", "goodbye", "bye"}));
}


