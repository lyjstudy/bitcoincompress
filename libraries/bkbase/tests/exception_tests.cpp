#include <bkbase/exception.h>
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>

BK_DEFINE_ERROR_INFO(TestString, std::string);
BK_DEFINE_ERROR_INFO(TestInteger, int);

BOOST_AUTO_TEST_SUITE(exception_tests)

BOOST_AUTO_TEST_CASE(base) {
    try {
        BK_THROW(bkbase::Exception() << TestString("Hello") << TestInteger(100));
    } catch (bkbase::Exception &ex) {
        BOOST_CHECK_EQUAL(*boost::get_error_info<TestString>(ex), std::string("Hello"));
        BOOST_CHECK_EQUAL(*boost::get_error_info<TestInteger>(ex), 100);
        std::cerr << boost::diagnostic_information(ex) << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END()
