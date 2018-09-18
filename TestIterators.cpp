/*
 * TestIterators.cpp
 *
 *  Created on: 18 Sep 2018
 *      Author: suresh
 */

#include <iterator>
#include <type_traits>
#include <memory>
#include <iostream>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits.hpp>

#include "TestIterators.h"

namespace
{
    template<typename T>
    class Interface
    {
    public:
        Interface() {}

        // Virtual destructor, to ensure well-defined semantics
        virtual ~Interface() noexcept
        {
            BOOST_MPL_ASSERT((std::is_abstract<T>));
            BOOST_MPL_ASSERT((boost::has_virtual_destructor<T>));
            BOOST_MPL_ASSERT((boost::is_base_of<Interface, T>));
        }

    private:
        Interface(Interface const &) = delete;
        Interface & operator =(Interface const &) = delete;
    };

    template<typename V>
    class IteratorAccess: public Interface<IteratorAccess<V>>
    {
    public:
        typedef V value_type;

        virtual bool hasNext() const = 0;
        virtual value_type * next() = 0;
    };

    template<typename V>
    class Iterator: public boost::iterator_facade<Iterator<V>, V, boost::forward_traversal_tag>
    {
    public:
        Iterator(): current {nullptr} {}

        explicit Iterator(std::shared_ptr<IteratorAccess<V>> const & i): access { i }
        {
            increment();
        }

    private:
        friend class boost::iterator_core_access;

        void increment()
        {
            current = access->hasNext() ? access->next() : nullptr;
        }

        bool equal(Iterator const & other) const
        {
            return this->current == other.current;
        }

        V & dereference() const
        {
            return *current;
        }

        V * current;
        std::shared_ptr<IteratorAccess<V>> const access;
    };

    class StringIteratorAccess: public IteratorAccess<char const>
    {
    public:
        explicit StringIteratorAccess(std::string const & value): data{value}
        {
        }

        bool hasNext() const override
        {
            return nextIndex < data.length();
        }

        virtual value_type * next() override
        {
            return &data[nextIndex++];
        }

    private:
        std::string const data;
        size_t nextIndex = 0;
    };
}

void suriar::testIterators()
{
    std::cout << "Iterators tests-------------------\n";

    auto access1 = std::make_shared<StringIteratorAccess>("KLM");
    auto access2 = std::make_shared<StringIteratorAccess>("");

    auto const begin = Iterator<char const> { access1 };
    auto const end = Iterator<char const> { };

    std::cout << "Iterators: KLM test\n";
    for (auto i = begin; i != end; ++i)
    {
        std::cout << *i << std::endl;
    }
    std::cout << "Iterators: empty test\n";
    for (auto i = Iterator<char const> { access2 }; i != end; ++i)
    {
        std::cout << *i << std::endl;
    }
    std::cout << "Iterators: End\n";
}
