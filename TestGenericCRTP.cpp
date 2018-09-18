/*
 * TestGenericCRTP.cpp
 *
 *  Created on: 18 Sep 2018
 *      Author: suresh
 */

#include "boost/mpl/empty_base.hpp"
#include "boost/operators.hpp"
#include <cassert>
#include <iostream>

#include "TestGenericCRTP.h"

namespace suriar
{
	namespace Representation
	{
		template <typename D, typename R, typename T = boost::mpl::empty_base>
		class Traits: public T
		{
		public:
			typedef D derived_type;
			typedef R representation_type;

		protected:
			D const & toDerived() const
			{
				return *static_cast<D const *>(this);
			}

			const R toRepresentation() const
			{
				return toDerived().toRepresentation();
			}
		};

		template <typename T>
		struct TotallyOrdered: public boost::totally_ordered1<typename T::derived_type, T>
		{
			bool operator <(typename T::derived_type const & other) const
			{
				return T::toRepresentation() < other.toRepresentation();
			}

			bool operator ==(typename T::derived_type const & other) const
			{
				return T::toRepresentation() == other.toRepresentation();
			}
		};

		template <const char * name, typename T>
		struct Streamable: public T
		{
			friend std::ostream & operator <<(std::ostream & stream, typename T::derived_type const & v)
			{
				stream << name << '[' << v.toRepresentation() << ']';
				return stream;
			}
		};

		template <typename T>
		struct Unboxable: public T
		{
			operator typename T::representation_type() const
			{
				return T::toRepresentation();
			}
		};

		template <typename T>
		class Pod: public T
		{
		public:
			typedef Pod super;

			explicit Pod(typename T::representation_type && value): representation_value{value} {}

			const typename T::representation_type toRepresentation() const { return representation_value; }

		private:
			typename T::representation_type const representation_value;
		};
	}
}

using namespace suriar;
using namespace suriar::Representation;

namespace
{
	extern char const ageName[] = "Age";

	class Age: public Pod<Streamable<ageName,TotallyOrdered<Unboxable<Traits<Age,int>>>>> { public: using super::super;};
	class Height: public Pod<Streamable<ageName,TotallyOrdered<Traits<Height,int>>>> { public: using super::super;};

	void printAge(Age const & age)
	{
		std::cout << "Age = " << age << std::endl;
	}

	void printAll(Age const & age, Height const & height)
	{
		std::cout << "Age = " << age << ", Height = " << height << std::endl;
	}

	// Test unbox operator;
	Age agePlus50(Age const & age) { return Age(age + 50); }
}

void suriar::testGenericCRTP()
{
	Age a1(23);
	Age a2(32);

	assert(a1 < a2);

	if (a1 < a2) std::cout << "pass" << std::endl;

	printAge(a1);
	printAge(agePlus50(a2));

	printAll(Age{23}, Height{155});
}
