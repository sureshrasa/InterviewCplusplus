/*
 * TestTrie.cpp
 *
 *  Created on: 18 Sep 2018
 *      Author: suresh
 */

#include "TestTrie.h"
#include "Interface.hpp"
#include "boost/range/iterator_range.hpp"
#include "boost/optional.hpp"
#include <memory>
#include <map>
#include <utility>
#include <iterator>
#include <string>
#include <iostream>


namespace suriar
{
    template <typename K, typename V>
    class Trie: public Interface<Trie<K,V>>
    {
    public:
        typedef typename K::const_iterator key_iterator;
        typedef boost::iterator_range<key_iterator> iterator_range;

        typedef std::shared_ptr<Trie const> const_pointer_type;
        typedef std::shared_ptr<Trie> pointer_type;

        typedef std::pair<iterator_range, boost::optional<V>> result_type;

        virtual bool hasChildren() const = 0;
        virtual boost::optional<V> const getValue() const = 0;

        virtual result_type longest_prefix(K const & key) const = 0;
        pointer_type put(K const & key, V const & value)
        {
            return put(boost::make_iterator_range(std::begin(key), std::end(key)), value);
        }

        virtual pointer_type operator[](typename K::value_type const & digit) = 0;
        virtual const_pointer_type operator[](typename K::value_type const & digit) const = 0;
        virtual pointer_type put(iterator_range const & range, V const & value) = 0;
   };

    template <typename K, typename V>
    class EmptyTrie:
            public Trie<K,V>,
            public std::enable_shared_from_this<Trie<K,V>>
    {
    public:
        static EmptyTrie<K,V> INSTANCE;

        static std::shared_ptr<Trie<K,V>> instance()
        {
            return std::shared_ptr<Trie<K,V>>(&INSTANCE, [](Trie<K,V> * p){});
        }

        bool hasChildren() const override { return false; }
        boost::optional<V> const getValue() const override { return {}; }

        typename Trie<K,V>::result_type longest_prefix(K const & key) const override
        {
            return std::make_pair(
                    boost::make_iterator_range(std::end(key), std::end(key)),
                    boost::optional<V>()
                    );
        }

        virtual std::shared_ptr<Trie<K,V>> operator[](typename K::value_type const &) override
        {
            return std::enable_shared_from_this<Trie<K,V>>::shared_from_this();
        }

        virtual std::shared_ptr<Trie<K,V> const> operator[](typename K::value_type const &) const override
        {
            return std::enable_shared_from_this<Trie<K,V>>::shared_from_this();
        }

    public:
        std::shared_ptr<Trie<K,V>> put(typename Trie<K,V>::iterator_range const & range, V const & value) override;
    };

    template <typename K, typename V>
    class NodeTrie:
            public Trie<K,V>,
            public std::enable_shared_from_this<Trie<K,V>>
    {
    public:
        bool hasChildren() const override { return true; }
        boost::optional<V> const getValue() const override { return value; }

        typename Trie<K,V>::result_type longest_prefix(K const & key) const override
        {
            auto const begin = std::begin(key);
            auto const end = std::end(key);
            auto trie = std::enable_shared_from_this<Trie<K,V>>::shared_from_this();
            for(auto i = begin; i != end; ++i)
            {
                auto const child = (*trie)[*i];
                if (!child->hasChildren())
                {
                    return std::make_pair(boost::make_iterator_range(begin, i), trie->getValue());
                }
                trie = child;
            }
            return std::make_pair(boost::make_iterator_range(end, end), trie->getValue());
        }

        std::shared_ptr<Trie<K,V>> operator[](typename K::value_type const & digit) override
        {
            auto const pos = children.find(digit);

            return (pos == children.end()) ? EmptyTrie<K,V>::instance() : pos->second;
        }

        std::shared_ptr<Trie<K,V> const> operator[](typename K::value_type const & digit) const override
        {
            auto const pos = children.find(digit);

            return (pos == children.end()) ? EmptyTrie<K,V>::instance() : pos->second;
        }

        std::shared_ptr<Trie<K,V>> put(typename Trie<K,V>::iterator_range const & range, V const & value) override
        {
            auto const curr = std::begin(range);
            if (curr == std::end(range))
            {
                this->value = value;
            }
            else
            {
                auto digit = *curr;
                auto pos = children.find(digit);
                if (pos == children.end())
                {
                    children[digit] = std::make_shared<NodeTrie<K,V>>();
                }
                children[digit]->put(boost::make_iterator_range(std::next(curr), std::end(range)), value);
            }
            return std::enable_shared_from_this<Trie<K,V>>::shared_from_this();
        }

    private:
        boost::optional<V> value;
        std::map<typename K::value_type, std::shared_ptr<Trie<K,V>>> children;
    };

    template <typename K, typename V>
    std::shared_ptr<Trie<K,V>> EmptyTrie<K,V>::put(typename Trie<K,V>::iterator_range const & range, V const & value)
    {
        auto const curr = std::begin(range);
        if (curr == std::end(range))
        {
            return std::enable_shared_from_this<Trie<K,V>>::shared_from_this();
        }
        else
        {
            auto const trie = std::make_shared<NodeTrie<K,V>>();

            trie->put(range, value);

            return trie;
        }
    }

    template <typename K, typename V>
    EmptyTrie<K,V> EmptyTrie<K,V>::INSTANCE;

}

void suriar::testTrie()
{
    std::shared_ptr<Trie<std::string, unsigned>> trie = std::make_shared<EmptyTrie<std::string, unsigned>>();

    assert(!trie->hasChildren());
    assert(!trie->longest_prefix("hello").second);

    trie = trie->put("hello", 23);
    assert(trie->longest_prefix("hello").second);
    assert(*trie->longest_prefix("hello").second==23);
    assert(!trie->longest_prefix("hell").second);

    trie = trie->put("hello!", 32);
    assert(trie->longest_prefix("hello").second);
    assert(*trie->longest_prefix("hello").second==23);
    assert(trie->longest_prefix("hello!").second);
    assert(*trie->longest_prefix("hello!").second==32);

    trie = trie->put("hell", 999);
    assert(trie->longest_prefix("hell").second);
    assert(*trie->longest_prefix("hell").second==999);
    assert(trie->longest_prefix("hello").second);
    assert(*trie->longest_prefix("hello").second==23);
    assert(trie->longest_prefix("hello!").second);
    assert(*trie->longest_prefix("hello!").second==32);

    std::cout << "Trie tests complete....\n";
}


