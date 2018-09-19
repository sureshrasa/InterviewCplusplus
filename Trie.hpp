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
    private:
        class TrieElement: public Interface<TrieElement>
        {
        public:
            typedef typename K::const_iterator key_iterator;
            typedef boost::iterator_range<key_iterator> iterator_range;
            typedef std::pair<iterator_range, boost::optional<V>> result_type;

            typedef std::shared_ptr<TrieElement const> const_pointer_type;
            typedef std::shared_ptr<TrieElement> pointer_type;

            virtual bool hasChildren() const = 0;
            virtual boost::optional<V> const getValue() const = 0;
            virtual result_type longest_prefix(K const & key) = 0;

            virtual pointer_type operator[](typename K::value_type const & digit) = 0;
            virtual pointer_type put(iterator_range const & range, V const & value) = 0;
        };

        class EmptyTrieElement: public TrieElement
        {
        public:
            static std::shared_ptr<TrieElement> instance()
            {
                static std::shared_ptr<TrieElement> INSTANCE;

                if (!INSTANCE)
                {
                    INSTANCE = std::make_shared<EmptyTrieElement>();
                }
                return INSTANCE;
            }

            bool hasChildren() const override { return false; }
            boost::optional<V> const getValue() const override { return  {}; }

            typename HashTrie::result_type longest_prefix(K const & key) override
            {
                return std::make_pair(boost::make_iterator_range(std::end(key),std::end(key)), boost::optional<V>());
            }

            virtual typename TrieElement::pointer_type operator[](typename K::value_type const &) override
            {
                return instance();
            }

            typename TrieElement::pointer_type put(typename TrieElement::iterator_range const & range, V const & value) override;
        };

        class NodeTrieElement:
                public TrieElement,
                public std::enable_shared_from_this<TrieElement>
        {
        public:
            bool hasChildren() const override { return true; }

            boost::optional<V> const getValue() const override { return value; }

            typename TrieElement::result_type longest_prefix(K const & key) override
            {
                auto const begin = std::begin(key);
                auto const end = std::end(key);
                auto trie = std::enable_shared_from_this<TrieElement>::shared_from_this();
                for (auto i = begin; i != end; ++i)
                {
                    auto const child = (*trie)[*i];
                    if (!child->hasChildren())
                    {
                        return std::make_pair(boost::make_iterator_range(begin, i), trie->getValue());
                    }
                    trie = child;
                }
                return std::make_pair(boost::make_iterator_range(begin, end), trie->getValue());
            }

            typename TrieElement::pointer_type operator[](typename K::value_type const & digit) override
            {
                auto const pos = children.find(digit);

                return (pos == children.end()) ? EmptyTrieElement::instance() : pos->second;
            }

            typename TrieElement::pointer_type put(typename TrieElement::iterator_range const & range, V const & value) override
            {
                auto const curr = std::begin(range);
                if (curr == std::end(range))
                {
                    this->value = value;
                }
                else
                {
                    auto & child = children[*curr];
                    if (!child)
                    {
                        child = std::make_shared<NodeTrieElement>();
                    }
                    child = child->put(boost::make_iterator_range(std::next(curr), std::end(range)), value);
                }
                return std::enable_shared_from_this<TrieElement>::shared_from_this();
            }

        private:
            boost::optional<V> value;
            std::map<typename K::value_type, typename TrieElement::pointer_type> children;
        };

        typename TrieElement::pointer_type root;
    public:
        HashTrie(): root{EmptyTrieElement::instance()} {}

        typename Trie<K,V>::result_type longest_prefix(K const & key) const override
        {
            return root->longest_prefix(key);
        }

        void put(K const & key, V const & value) override
        {
            root = root->put(boost::make_iterator_range(std::begin(key), std::end(key)), value);
        }
    };

    template<typename K, typename V>
    typename HashTrie<K, V>::TrieElement::pointer_type HashTrie<K, V>::EmptyTrieElement::put(
            typename HashTrie<K, V>::TrieElement::iterator_range const & range,
            V const & value)
    {
        auto const curr = std::begin(range);
        if (curr == std::end(range))
        {
            return instance();
        }
        else
        {
            auto const trie = std::make_shared<NodeTrieElement>();

            (void)trie->put(range, value);

            return trie;
        }
    }
}

#endif /* TRIE_HPP_ */
