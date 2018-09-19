/*
 * TestTrie.cpp
 *
 *  Created on: 18 Sep 2018
 *      Author: suresh
 */

#include "TestTrie.h"
#include "Trie.hpp"
#include <iostream>

void suriar::testTrie()
{
    std::cout << "Trie tests....\n";

    auto const trie = std::make_shared<HashTrie<std::string, unsigned>>();

    {
        std::string const key = "hello";
        auto const result = trie->longest_prefix(key);
        assert(!result.second);
        assert(std::begin(result.first) == key.end());
        auto const match = std::string(std::begin(result.first), std::end(result.first));
        assert(match == "");
    }

    trie->put("hello", 23);
    assert(trie->longest_prefix("hello").second);
    assert(*trie->longest_prefix("hello").second==23);
    assert(!trie->longest_prefix("hell").second);

    trie->put("hello!", 32);
    assert(trie->longest_prefix("hello").second);
    assert(*trie->longest_prefix("hello").second==23);
    assert(trie->longest_prefix("hello!").second);
    assert(*trie->longest_prefix("hello!").second==32);

    trie->put("hell", 999);
    assert(trie->longest_prefix("hell").second);
    assert(*trie->longest_prefix("hell").second==999);
    assert(trie->longest_prefix("hello").second);
    assert(*trie->longest_prefix("hello").second==23);
    assert(trie->longest_prefix("hello!").second);
    assert(*trie->longest_prefix("hello!").second==32);
    {
        std::string const key = "hello! how are you?";
        auto const result = trie->longest_prefix(key);
        assert(result.second);
        assert(*result.second==32);
        auto const match = std::string(std::begin(result.first), std::end(result.first));
        assert(match == "hello!");
    }

    {
        std::string const key = "hello world! how are you?";
        auto const result = trie->longest_prefix(key);
        assert(result.second);
        assert(*result.second==23);
        auto const match = std::string(std::begin(result.first), std::end(result.first));
        assert(match == "hello");
    }

    {
        std::string const key = "honey; how are you?";
        auto const result = trie->longest_prefix(key);
        assert(!result.second);
        auto const match = std::string(std::begin(result.first), std::end(result.first));
        assert(match == "h");
    }

    {
        std::string const key = "are you ok?";
        auto const result = trie->longest_prefix(key);
        assert(!result.second);
        assert(std::end(result.first) == key.begin());
        auto const match = std::string(std::begin(result.first), std::end(result.first));
        assert(match == "");
    }

    std::cout << "Trie tests complete....\n";
}

