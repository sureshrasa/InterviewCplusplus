/*
 * TestMove.cpp
 *
 *  Created on: 18 Sep 2018
 *      Author: suresh
 */

#include <map>
#include <optional>
#include <string>
#include <iostream>
#include <initializer_list>
#include "boost/optional.hpp"
#include "boost/optional/optional_io.hpp"

#include "TestMove.h"

namespace
{
	template <typename K, typename V>
	class Cache //: boost::noncopyable
	{
	private:
		typedef std::map<K,V> map_type;

	public:
		typedef typename map_type::value_type value_type;

		Cache(Cache const &) = delete;
		Cache & operator =(Cache const &) = delete;

		explicit Cache(std::initializer_list<value_type> && entries): map{entries} {}

		Cache(Cache && other)
		{
			std::swap(map, other.map);
		}

		Cache & operator =(Cache && other)
		{
			std::swap(map, other.map);
		}

		boost::optional<V> get(K const & key) const
		{
			auto const pos = map.find(key);
			return (pos == map.end()) ? boost::optional<V>{} : pos->second;
		}

		void put(K const & key, V const & value)
		{
			map[key] = value;
		}

		auto begin() const { return map.begin(); }
		auto end() const { return map.end(); }

	private:
		map_type map;
	};

	template <typename K, typename V>
	void printCache(Cache<K,V> const & cache)
	{
		std::cout << "Cache[ ";
		for (auto e : cache)
		{
			std::cout << "[" << e.first << ", " << e.second << "] ";
		}
		std::cout << "]" << std::endl;
	}
}

void suriar::testMove()
{
	typedef Cache<int, std::string> StrCache;

	StrCache c1{};

	c1.put(1, "A");
	c1.put(2, "B");

	printCache(c1);

	StrCache c2 = std::move(c1);
	printCache(c1);
	printCache(c2);

	StrCache c3{std::move(c2)};
	printCache(c1);
	printCache(c2);
	printCache(c3);

	StrCache c4{{3, "C"}, {4, "D"}};
	printCache(c4);
}
