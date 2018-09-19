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
#include <exception>

#include "Interface.hpp"

namespace suriar
{
    template<typename K, typename V>
    class Trie: public Interface<Trie<K, V>>
    {
    public:
        typedef boost::iterator_range<typename K::const_iterator> iterator_range;

        typedef std::pair<iterator_range, boost::optional<V>> result_type;

        virtual bool isEmpty() const = 0;
        virtual result_type longest_prefix_path(iterator_range const & path) const = 0;
        virtual void put(K const & key, V const & value) = 0;

        virtual result_type longest_prefix(K const & key) const
        {
            return longest_prefix_path(boost::make_iterator_range(std::begin(key), std::end(key)));
        }
   };

    template<typename K, typename V> bool Trie<K,V>::isEmpty() const { return true; }
    template<typename K, typename V>
    std::pair<boost::iterator_range<typename K::const_iterator>, boost::optional<V>> Trie<K,V>::longest_prefix_path(typename Trie<K,V>::iterator_range const & key) const
    {
        return std::make_pair(boost::make_iterator_range(std::begin(key), std::end(key)), boost::optional<V>{});
    }
    template<typename K, typename V>
    void Trie<K,V>::put(K const &, V const &)
    {
        throw std::runtime_error("Trie::put is Unsupported");
    }

    template<typename K, typename V>
    class HashTrie: public Trie<K, V>
    {
    public:
        virtual bool isEmpty() const override { return Trie<K,V>::isEmpty(); }

        typename Trie<K, V>::result_type longest_prefix_path(typename Trie<K,V>::iterator_range const & path) const override
        {
            auto const begin = std::begin(path);
            auto const end = std::end(path);
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
