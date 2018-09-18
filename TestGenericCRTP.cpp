/*
 * TestGenericCRTP.cpp
 *
 *  Created on: 18 Sep 2018
 *      Author: suresh
 */

#include <string>
#include <memory>
#include <cassert>
#include <iostream>

#include "boost/mpl/empty_base.hpp"
#include "boost/operators.hpp"
#include "boost/mpl/assert.hpp"
#include "boost/type_traits/is_base_of.hpp"

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
				BOOST_MPL_ASSERT((boost::is_base_of<Traits, D>));
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

		template <typename T>
		struct Streamable: public T
		{
			friend std::ostream & operator <<(std::ostream & stream, typename T::derived_type const & v)
			{
				v.output(stream);
				return stream;
			}
		};

		template <const char * name, typename T>
		struct TaggedStreamable: public Streamable<T>
		{
			void output(std::ostream & stream) const
			{
				stream << name << '[' << T::toRepresentation() << ']';
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

			explicit Pod(typename T::representation_type && v): value(std::forward<typename T::representation_type>(v)) {}

			const typename T::representation_type toRepresentation() const { return value; }

		private:
			typename T::representation_type const value;
		};

		template <typename P, typename T>
		class Ref: public T
		{
		public:
			typedef Ref super;

			explicit Ref(P && v): valuePtr{std::forward<P>(v)} {}

			const typename T::representation_type toRepresentation() const { return *valuePtr; }

		private:
			P const valuePtr;
		};
	}
}

using namespace suriar;
using namespace suriar::Representation;

namespace
{
	extern char const ageName[] = "Age";
	extern char const heightName[] = "Height";
	extern char const nameName[] = "Name";

	class Age: public Pod<TaggedStreamable<ageName,TotallyOrdered<Unboxable<Traits<Age,int>>>>> { public: using super::super;};
	class Height: public Pod<TaggedStreamable<heightName,TotallyOrdered<Traits<Height,int>>>> { public: using super::super;};

	class Name: public Ref<std::shared_ptr<std::string>, TaggedStreamable<nameName,TotallyOrdered<Traits<Name,std::string>>>> { public: using super::super;};

	class Address: public Ref<std::unique_ptr<std::string>, Streamable<TotallyOrdered<Traits<Address,std::string>>>>
	{
	public:
		using super::super;

		void output(std::ostream & stream) const
		{
			stream << "Addr<" << toRepresentation() << ">\n";
		}
	};

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

	constexpr int fibonacciAge(int const i)
	{
		switch (i)
		{
		case 0: return 1;
		case 1: return 1;
		default: return fibonacciAge(i-1)+fibonacciAge(i-2);
		}
	}
}

void suriar::testGenericCRTP()
{
	Age a1(23);
	Age a2(fibonacciAge(10));

	assert(a1 < a2);

	if (a1 < a2) std::cout << "pass" << std::endl;

	printAge(a1);
	printAge(agePlus50(a2));

	printAll(Age{23}, Height{155});

	if (Name{std::make_shared<std::string>("abc")} < Name{std::make_shared<std::string>("xyz")}) std::cout << "pass" << std::endl;

	std::cout << "My name is " << Name{std::make_shared<std::string>("Suresh")} << std::endl;
	std::cout << "My address is " << Address{std::make_unique<std::string>("London")} << std::endl;

	auto nyc = std::make_unique<std::string>("New York");
	std::cout << "My address is " << Address{std::move(nyc)} << std::endl;
	assert(nyc == nullptr);
}
