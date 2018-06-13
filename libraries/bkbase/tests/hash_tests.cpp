#include <bkbase/hash.h>

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(hash_tests)

BOOST_AUTO_TEST_CASE(logic) {
    bkbase::Hash256 hash256;
    hash256.SetZero();
    BOOST_CHECK(hash256.IsZero());
}

BOOST_AUTO_TEST_SUITE_END()
