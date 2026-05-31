#include "bmstu_unordered_map.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

TEST(UnorderedMapTest, DefaultConstructedIsEmpty)
{
	bmstu::unordered_map<int, int> m;
	EXPECT_TRUE(m.empty());
	EXPECT_EQ(m.size(), 0u);
}

TEST(UnorderedMapTest, InsertAndAccess)
{
	bmstu::unordered_map<int, std::string> m;
	m[1] = "one";
	m[2] = "two";
	m[3] = "three";

	EXPECT_EQ(m[1], "one");
	EXPECT_EQ(m[2], "two");
	EXPECT_EQ(m[3], "three");
	EXPECT_EQ(m.size(), 3u);
}

TEST(UnorderedMapTest, InsertReturnValue)
{
	bmstu::unordered_map<int, int> m;

	auto [it1, inserted1] = m.insert({42, 100});
	EXPECT_TRUE(inserted1);
	EXPECT_EQ(it1->first, 42);
	EXPECT_EQ(it1->second, 100);

	auto [it2, inserted2] = m.insert({42, 999});
	EXPECT_FALSE(inserted2);
	EXPECT_EQ(m[42], 100);
}

TEST(UnorderedMapTest, FindExistingKey)
{
	bmstu::unordered_map<std::string, int> m;
	m["apple"] = 5;
	m["banana"] = 10;
	m["cherry"] = 15;

	auto it = m.find("apple");
	EXPECT_NE(it, m.end());
	EXPECT_EQ(it->second, 5);

	it = m.find("banana");
	EXPECT_NE(it, m.end());
	EXPECT_EQ(it->second, 10);
}

TEST(UnorderedMapTest, FindMissingKey)
{
	bmstu::unordered_map<std::string, int> m;
	m["apple"] = 5;

	auto it = m.find("grape");
	EXPECT_EQ(it, m.end());
}

TEST(UnorderedMapTest, Contains)
{
	bmstu::unordered_map<int, double> m;
	m[1] = 1.5;
	m[2] = 2.5;
	m[3] = 3.5;

	EXPECT_TRUE(m.contains(1));
	EXPECT_TRUE(m.contains(2));
	EXPECT_TRUE(m.contains(3));
	EXPECT_FALSE(m.contains(4));
	EXPECT_FALSE(m.contains(0));
}

TEST(UnorderedMapTest, EraseExistingKey)
{
	bmstu::unordered_map<int, std::string> m;
	m[5] = "five";
	m[6] = "six";
	m[7] = "seven";

	EXPECT_TRUE(m.erase(6));
	EXPECT_EQ(m.size(), 2u);
	EXPECT_FALSE(m.contains(6));
	EXPECT_TRUE(m.contains(5));
	EXPECT_TRUE(m.contains(7));
}

TEST(UnorderedMapTest, EraseMissingKey)
{
	bmstu::unordered_map<int, int> m;
	m[1] = 10;

	EXPECT_FALSE(m.erase(99));
	EXPECT_EQ(m.size(), 1u);
}

TEST(UnorderedMapTest, EraseAllElements)
{
	bmstu::unordered_map<int, int> m;
	for (int i = 0; i < 10; ++i)
		m[i] = i * 2;

	for (int i = 0; i < 10; ++i)
		m.erase(i);

	EXPECT_TRUE(m.empty());
	EXPECT_EQ(m.size(), 0u);
}

TEST(UnorderedMapTest, AtExistingKey)
{
	bmstu::unordered_map<int, std::string> m;
	m[10] = "ten";

	EXPECT_EQ(m.at(10), "ten");
}

TEST(UnorderedMapTest, AtMissingKeyThrows)
{
	bmstu::unordered_map<int, std::string> m;
	EXPECT_THROW(m.at(42), std::out_of_range);
}

TEST(UnorderedMapTest, AtConstRef)
{
	bmstu::unordered_map<std::string, int> m;
	m["x"] = 7;
	const auto& cm = m;
	EXPECT_EQ(cm.at("x"), 7);
	EXPECT_THROW(cm.at("y"), std::out_of_range);
}

TEST(UnorderedMapTest, UpdateValue)
{
	bmstu::unordered_map<std::string, int> m;
	m["counter"] = 0;
	EXPECT_EQ(m["counter"], 0);

	m["counter"] = 42;
	EXPECT_EQ(m["counter"], 42);

	m["counter"]++;
	EXPECT_EQ(m["counter"], 43);
	EXPECT_EQ(m.size(), 1u);
}

TEST(UnorderedMapTest, StringKeys)
{
	bmstu::unordered_map<std::string, std::string> m;
	m["red"] = "#FF0000";
	m["green"] = "#00FF00";
	m["blue"] = "#0000FF";

	EXPECT_EQ(m["red"], "#FF0000");
	EXPECT_EQ(m["green"], "#00FF00");
	EXPECT_EQ(m["blue"], "#0000FF");
	EXPECT_EQ(m.size(), 3u);
}

TEST(UnorderedMapTest, Clear)
{
	bmstu::unordered_map<int, int> m;
	for (int i = 0; i < 20; ++i)
		m[i] = i;

	m.clear();
	EXPECT_TRUE(m.empty());
	EXPECT_EQ(m.size(), 0u);

	m[1] = 100;
	EXPECT_EQ(m[1], 100);
	EXPECT_EQ(m.size(), 1u);
}

TEST(UnorderedMapTest, IterationCoversAllElements)
{
	bmstu::unordered_map<int, int> m;
	std::set<int> expected;

	for (int i = 1; i <= 10; ++i)
	{
		m[i] = i * 10;
		expected.insert(i);
	}

	std::set<int> visited;
	for (auto it = m.begin(); it != m.end(); ++it)
	{
		EXPECT_EQ(it->second, it->first * 10);
		visited.insert(it->first);
	}

	EXPECT_EQ(visited, expected);
}

TEST(UnorderedMapTest, RangeForLoop)
{
	bmstu::unordered_map<std::string, int> m;
	m["a"] = 1;
	m["b"] = 2;
	m["c"] = 3;

	int sum = 0;
	for (const auto& [key, val] : m)
		sum += val;

	EXPECT_EQ(sum, 6);
}

TEST(UnorderedMapTest, EmptyMapIteration)
{
	bmstu::unordered_map<int, int> m;
	EXPECT_EQ(m.begin(), m.end());
}

TEST(UnorderedMapTest, RehashPreservesAllElements)
{
	bmstu::unordered_map<int, int> m(4);

	for (int i = 0; i < 50; ++i)
		m[i] = i * 3;

	EXPECT_EQ(m.size(), 50u);
	for (int i = 0; i < 50; ++i)
	{
		EXPECT_TRUE(m.contains(i));
		EXPECT_EQ(m[i], i * 3);
	}
}

TEST(UnorderedMapTest, LoadFactorBelowThreshold)
{
	bmstu::unordered_map<int, int> m;
	for (int i = 0; i < 100; ++i)
		m[i] = i;

	EXPECT_LE(m.load_factor(), 0.75);
}

TEST(UnorderedMapTest, ReserveIncreasesCapacity)
{
	bmstu::unordered_map<int, int> m;
	m.reserve(1000);
	EXPECT_GE(m.bucket_count(), static_cast<std::size_t>(1000 / 0.75));
}

TEST(UnorderedMapTest, StreebogHashDistribution)
{
	bmstu::unordered_map<int, int> m;
	constexpr int N = 1000;

	for (int i = 0; i < N; ++i)
		m[i] = i;

	EXPECT_EQ(m.size(), static_cast<std::size_t>(N));

	for (int i = 0; i < N; ++i)
		EXPECT_EQ(m.at(i), i);

	double avg =
		static_cast<double>(m.size()) / static_cast<double>(m.bucket_count());
	EXPECT_LE(avg, 2.0);
}

// структура машины обычная
struct Car
{
	std::string vin;
	std::string model;
	double weight;

	// делаем оператор == для того, чтобы работал
	// static_assert(has_equality_operator_v<K>,
	//			  "bmstu::unordered_map: key type must provide `operator==`.");
	bool operator==(const Car& other) const
	{
		return vin == other.vin && model == other.model &&
			   weight == other.weight;
	}

	// делаем rawBytes для того, чтобы работал SFINAE
	std::vector<uint8_t> rawBytes() const
	{
		// буфер
		std::string raw = vin + model + std::to_string(weight);
		return std::vector<uint8_t>(raw.begin(), raw.end());
	}
};

TEST(UnorderedMapTest, CarsDict)
{
	// мап из каров
	bmstu::unordered_map<Car, std::string> cars;
	cars[Car{"body1", "gto", 2500}] = "SportCar";
	cars[Car{"body2", "duster", 1850}] = "CrossOver";
	cars[Car{"body3", "2111", 1850}] = "Universal";

	// изначально все типы в unordered_map записываются не по порядку
	// Ершов просит сделать чтобы они выводились по порядку

	// bmstu::unordered_map<std::size_t, std::string> ids;
	// ids[1] = "Hm1";
	// ids[2] = "Ogo2";
	// ids[4] = "Vitya4";
	// ids[5] = "Imarobot";

	// например тут без доп методов и обработки будет выводиться все в рандомном
	// порядке чтобы такого не происходило, нужно перезгрузить в файле .h
	// шаблон, отвечающий за хэширование (.h, строка 164)

	bmstu::unordered_map<double, std::string> ids;
	ids[2.654] = "Sasha1";
	ids[0.006666] = "Sasha2";
	ids[2.5] = "Petya";
	ids[2.55] = "Vasya";
	ids[2.56] = "Kolya";
	ids[2.65] = "Sasha3";

	for (const auto& [k, v] : ids)
	{
		std::cout << "ID:" << k << " Name: " << v << std::endl;
	}
}