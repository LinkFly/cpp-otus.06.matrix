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

bool test_matrix() {
	const int count = 10000;
	return call_test(__PRETTY_FUNCTION__, []() {
		
		return true;
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
	BOOST_CHECK(test_matrix());
}

BOOST_AUTO_TEST_SUITE_END()
