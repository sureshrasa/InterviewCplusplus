/*
 * TestVariadicTemplates.cpp
 *
 *  Created on: 17 Sep 2018
 *      Author: suresh
 */

#include <vector>
#include <map>
#include <initializer_list>
#include <iostream>

#include "TestVariadicTemplates.h"

namespace
{
	template <typename T> void ignore(T) {}

	template <class ContainerType>
	class ContainerDecorator: public ContainerType
	{
	public:
		template <typename... Ts>
		ContainerDecorator(Ts const &... values): ContainerType{values...}
		{
			std::cout << "ContainerDecorator A\n";
		}

		ContainerDecorator(std::initializer_list<typename ContainerType::value_type> && values): ContainerType(std::move(values))
		{
			std::cout << "ContainerDecorator B\n";
		}

		template <typename... Ts>
		ContainerDecorator(typename ContainerType::value_type const & v, Ts const &... values)
		{
			std::cout << "ContainerDecorator C\n";

			*this << v;
			auto x = {(operator <<(values), 0)...}; ignore(x);
		}

		auto operator <<(typename ContainerType::value_type const & value)
		{
			std::cout << "<< " << value << "\n";

			ContainerType::push_back(std::move(value));
			return *this;
		}

		std::ostream & output(std::ostream & stream) const
		{
			for (typename ContainerType::value_type e : *this)
			{
				stream << e << "\n";
			}
			return stream;
		}
	};

	template <typename T> std::ostream & operator <<(std::ostream & stream, ContainerDecorator<T> const & c)
	{
		return c.output(stream);
	}

	template <typename C> class DerivedContainer: public ContainerDecorator<C>
	{
	public:
		using ContainerDecorator<C>::ContainerDecorator;
	};
}

void suriar::testVariadicTemplates()
{
	typedef ContainerDecorator<std::vector<int>> Container;

	Container c1(1, 2, 3);
	Container c2{9, 8, 7, 6, 5};
	Container c3{};
	DerivedContainer<std::vector<int>> c4{100, 200, 300};

	std::cout << "Element1:\n" << c1 << "Element 2:\n" << c2 << "Element 3:\n" << c3 << "Element 4:\n" << c4;

	for (auto i: {5, 4, 3, 2, 1})
	{
		std::cout << "Element initialiser-list: " << i << std::endl;
	}

	for (auto i: std::map<int,std::string>{{5,"a"}, {4, "b"}, {3, "c"}, {2, "d"}, {1, "e"}})
	{
		std::cout << "Element map: " << i.first << ' ' <<  i.second << "\n";
	}
}
