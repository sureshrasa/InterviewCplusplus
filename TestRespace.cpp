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
            auto const result = buildWords(std::get<2>(respace(std::numeric_limits<int>::max(), std::begin(str), std::end(str))));
            
            {
                std::cout << "respaced words [";
                std::copy(result.begin(), result.end(), std::ostream_iterator<std::string>(std::cout, " "));
                std::cout << "]\n";
            }

            cache.clear();

            return result;
        }

    private:
        typedef boost::iterator_range<std::string::const_iterator> word_range;
        typedef std::pair<bool, word_range> word_match;
        typedef std::vector<word_match> match_vector;

        std::shared_ptr<Dict const> const dict;
        std::map<int, std::tuple<int, int, match_vector>> cache;

        std::vector<std::string> buildWords(match_vector const & word_matches)
        {
            std::vector<std::string> result;

            std::transform(word_matches.begin(), word_matches.end(),
                    std::back_inserter(result),
                    [](auto const & s){return std::string(s.second.begin(), s.second.end());});

            return result;
        }

        auto const concatBadWord(word_match const & badWord, match_vector const & words)
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
        
        std::tuple<int, int, match_vector>
        respace(int const badCharLimit, std::string::const_iterator const & begin, std::string::const_iterator const & end)
        {
            std::cout << "respacing <" << std::string(begin, end) << "> limit =" << badCharLimit << "\n";

            if (begin == end) return std::make_tuple(0, 0, match_vector{});

            //if (badCharLimit < 0) return std::make_tuple(std::numeric_limits<int>::max(), 0, match_vector{});

            auto const searchPos = std::distance(begin, end);
            //auto const pos = cache.find(searchPos);
            //if (pos != cache.end()) return pos->second;

            auto bestSoFar = std::make_tuple(badCharLimit+1, std::numeric_limits<int>::max(), match_vector{});
            int wordLength = 0;
            for (auto i = begin; i != end; ++i, ++wordLength)
            {
                auto const match = dict->longest_prefix_path(boost::make_iterator_range(i, end));

                if (match.second)
                {
                    std::cout << "found good word<" << *match.second << "> bad word<" << std::string(begin, i) << "> badword len =" << wordLength << "\n";
                    auto const rest = respace(badCharLimit-wordLength, match.first.end(), end);
                    auto badWordCount = (wordLength > 0) ? 1 : 0;

                    if ( (std::get<0>(rest)+wordLength < std::get<0>(bestSoFar)) ||
                             (std::get<0>(rest)+wordLength == std::get<0>(bestSoFar) && std::get<1>(rest)+1+badWordCount < std::get<1>(bestSoFar)))
                    {
                        auto const bestWords = std::get<2>(bestSoFar);
                        std::cout << "respacing <" << std::string(match.first.end(), end) << "> better than <"
                                << std::string(bestWords[0].second.begin(), bestWords[0].second.end()) << ">\n";

                        auto const goodWord = std::make_pair(true, match.first);
                        auto const badWord = std::make_pair(false, boost::make_iterator_range(begin, i));
                        auto words = (badWordCount > 0)
                                ? concat(badWord, concat(goodWord, std::get<2>(rest)))
                                        : concat(goodWord, std::get<2>(rest));

                        bestSoFar = std::make_tuple(std::get<0>(rest)+wordLength, std::get<1>(rest)+1+badWordCount, words);
                    }
                 }
            }
            return registerResult(searchPos, bestSoFar);
         }

        std::tuple<int, int, match_vector> const registerResult(unsigned const searchPos, std::tuple<int, int, match_vector> const & result)
        {
            cache[searchPos] = result;
            return result;
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
    addWord(*dict, "byegones");
    addWord(*dict, "gone");

    auto respacer = std::make_shared<Respacer>(dict);

    assert((respacer->respace("jesslookedjustliketomherbrother") ==
            std::vector<std::string>{"jess", "looked", "just", "like", "tom", "her", "brother"}));

    assert((respacer->respace("goodbrothertomgoodbyebye") ==
            std::vector<std::string>{"good", "brother", "tom", "goodbye", "bye"}));

    assert((respacer->respace("goodbyegonesbebyegones") ==
            std::vector<std::string>{"good", "bygones", "be", "bygones"}));
}


