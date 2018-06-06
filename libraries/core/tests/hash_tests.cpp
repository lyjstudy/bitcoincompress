#include "hash.h"

#include <boost/test/unit_test.hpp>

using namespace core;

BOOST_AUTO_TEST_SUITE(hash_tests)

BOOST_AUTO_TEST_CASE(logic) {
    // BOOST_CHECK_EQUAL(sizeof(char), GetSerializeSize(char(0), 0));
    HashBase<8> hash;
    hash.SetZero();
    BOOST_CHECK(hash.IsZero());
    auto value = HashDoubleSha256("aaaaaaaaaa", 8);
    printf("%s\n", value.GetHex().c_str());
}

BOOST_AUTO_TEST_SUITE_END()
