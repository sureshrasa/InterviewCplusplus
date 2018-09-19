/*
 * Interface.hpp
 *
 *  Created on: 18 Sep 2018
 *      Author: suresh
 */

#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include <boost/iterator/iterator_facade.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits.hpp>

namespace suriar
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
}


#endif /* INTERFACE_HPP_ */
