/*
 * Trie.hpp
 *
 *  Created on: 19 Sep 2018
 *      Author: suresh
 */

#ifndef TRIE_HPP_
#define TRIE_HPP_

#include <boost/optional/optional.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <map>
#include <memory>
#include <utility>

#include "Interface.hpp"

namespace suriar
{
    template<typename K, typename V>
    class Trie: public Interface<Trie<K, V>>
    {
    public:
        typedef typename K::const_iterator key_iterator;
        typedef boost::iterator_range<key_iterator> iterator_range;

        typedef std::pair<iterator_range, boost::optional<V>> result_type;

        virtual result_type longest_prefix(K const & key) const = 0;
        virtual void put(K const & key, V const & value) = 0;
    };

    template<typename K, typename V>
    class HashTrie: public Trie<K, V>
    {
    public:
        typename Trie<K, V>::result_type longest_prefix(K const & key) const override
        {
            auto const begin = std::begin(key);
            auto const end = std::end(key);
            auto * trie = this;

            for (auto i = begin; i != end; ++i)
            {
                auto const pos = trie->children.find(*i);
                if (pos == trie->children.end())
                {
                    return std::make_pair(boost::make_iterator_range(begin, i), trie->value);
                }
                trie = pos->second.get();
            }
            return std::make_pair(boost::make_iterator_range(begin, end), trie->value);
        }

        void put(K const & key, V const & value) override
        {
            auto const end = std::end(key);
            auto * trie = this;
            for(auto i = std::begin(key); i != end; ++i)
            {
                auto & child = trie->children[*i];
                if (!child)
                {
                    child = std::make_shared<HashTrie>();
                }
                trie = child.get();
            }
            trie->value = value;
        }

    private:
        boost::optional<V> value;
        std::map<typename K::value_type, std::shared_ptr<HashTrie>> children;
    };
}

#endif /* TRIE_HPP_ */
