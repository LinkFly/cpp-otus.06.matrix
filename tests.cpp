#include "share.h"

#define BOOST_TEST_MODULE allocator_test_module

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <functional>
#include <ctime>
/*#include <map>
#include <numeric>*/

#include "matrix.h"

using std::string;
using std::cout;
using std::endl;
using std::clock;

bool call_test(string name, std::function<bool(void)> fntest) {
	cout << "------------------------------\n";
	cout << endl << name << ":\n";
	//test_data test_data = create_test_data();

	auto startTime = clock();
	bool res = fntest();
	auto endTime = clock();

	cout << "TIME: " << endTime - startTime << "ms" << endl;
	cout << "------------------------------\n";
	return res;
}

bool trivial_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		Matrix<int, -1> matrix; // бесконечная матрица int заполнена значениями -1
		assert(matrix.size() == 0); // все ячейки свободны
		auto a = matrix(0, 0);
		assert(a == -1);
		assert(matrix.size() == 0);
		matrix(100, 100) = 314;
		assert(matrix(100, 100) == 314);
		assert(matrix.size() == 1);

		return true;
	});
}

bool square_brackets_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		Matrix<int, 0> m;
		m[0][1] = 1;
		m[1][0] = 2;
		m[1][1] = 3;
		return m.size() == 3;
		});
}

bool forloop_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		Matrix<int, 0> m;
		m(0, 1) = 1;
		m(1, 0) = 2;
		m(3, 4) = 3;
		auto res = 0;
		for (auto& x : m) {
			res += std::get<1>(x);
		}
		return res == 6;
		});
}

bool coords_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		Matrix<int, 0> m;
		m(3, 4) = 3;
		m(5, 6) = 3;
		auto fnTestCoord = [](auto it, unsigned x, unsigned y) {
			auto coords = std::get<0>(*it);
			return std::get<0>(coords) == x && std::get<1>(coords) == y;
		};
		auto it = m.begin();
		return fnTestCoord(it, 3, 4) && fnTestCoord(++it, 5, 6);
		});
}


//struct Init {
//	Init(std::function<void()> init_func) {
//		init_func();
//	}
//};
//#define INIT(init_func) struct Init init(init_func);


BOOST_AUTO_TEST_SUITE(allocator_test_suite)
//INIT(init_base_fixtures)

BOOST_AUTO_TEST_CASE(test_of_matrix)
{
	BOOST_CHECK(trivial_test());
	BOOST_CHECK(square_brackets_test());
	BOOST_CHECK(forloop_test());
	BOOST_CHECK(coords_test());
}

BOOST_AUTO_TEST_SUITE_END()
