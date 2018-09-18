/*
 * TestConstructors.cpp
 *
 *  Created on: 18 Sep 2018
 *      Author: suresh
 */

#include "TestConstructors.h"

#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>

namespace
{
	template <typename C>
	class Container: public C
	{
	public:
		using C::C;

		Container(std::string const & s1, std::string const & s2): sample{-1}
		{
			std::cout << "Constructor Z:" << sample << ' ' << s1 << ':' << s2 << std::endl;
		}

		Container(std::string const & s): Container(s, "Bye bye universe...")
		{
			std::cout << "Constructor A:" << s << "\n";
		}

		Container(): Container("Hello World")
		{
			std::cout << "Constructor B\n";
		}

		std::ostream & output(std::ostream & stream) const
		{
			stream << '[';
			std::copy(std::begin(*this), std::end(*this), std::ostream_iterator<typename C::value_type>(stream, " "));
			stream << ']';

			return stream;
		}

		friend std::ostream & operator <<(std::ostream & stream, Container const & c)
		{
			return c.output(stream);
		}

	private:
		int const sample{999}; // Can only be initialised in non delegate constructors
	};
}

void suriar::testConstructors()
{
	std::cout << "TestConstructors:" << std::endl;
	std::cout << Container<std::vector<int>>{1,2,3,4,5} << std::endl;
	std::cout << Container<std::vector<int>>{} << std::endl;
	std::cout << Container<std::vector<int>>{"Bye bye..."} << std::endl;
}
