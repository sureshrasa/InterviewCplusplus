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
    auto const trie = std::make_shared<HashTrie<std::string, unsigned>>();

    assert(!trie->longest_prefix("hello").second);

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

    std::cout << "Trie tests complete....\n";
}


