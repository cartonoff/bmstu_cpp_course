#include <gtest/gtest.h>

#include <iostream>

#include "bmstu_unordered_map.h"

TEST(UnorderedMapTest, DefaultConstructor)
{
	for (int i = 0; i < 100; ++i)
	{
		auto hash = bmstu::hash<int>();
		std::cout << hash(i) << std::endl;
	}
}

TEST(UnorderedMapTest, Test)
{
	bmstu::hash<const char*> stringHasher;
	for (int i = 0; i < 100; ++i)
	{
		auto hash = bmstu::hash<int>();
		std::cout << hash(i) << std::endl;
	}
}

TEST(UnorderedMapTest, Tests5)
{
	bmstu::unordered_map<int, std::string> myMap;
	myMap.insert({10, "Everyone"});
	myMap.insert({1, "World"});
	myMap.insert({2, "Bar"});
	myMap.insert({3, "Qux"});
	myMap.insert({1, "Everyone"});
	myMap.insert({4, "Everyone"});
	myMap.insert({5, "Everyone"});
	for (const auto& [key, value] : myMap)
	{
		std::cout << key << " -> " << value << std::endl;
	}
}