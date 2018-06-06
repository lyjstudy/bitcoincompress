#include "hex.h"

#include <boost/test/unit_test.hpp>

using namespace core;

BOOST_AUTO_TEST_SUITE(hex_tests)

BOOST_AUTO_TEST_CASE(simple) {
    // BOOST_CHECK_EQUAL(sizeof(char), GetSerializeSize(char(0), 0));
    uint8_t buffer[16] = {
        0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78, 0x89, 0x9A, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xF0
    };
    std::string value = HexFromBin(buffer, sizeof(buffer));
    BOOST_CHECK_EQUAL(value, "0112233445566778899aabbccddeeff0");
    uint8_t out[16];
    size_t size = HexToBin(value.c_str(), out, 16);
    BOOST_CHECK_EQUAL(size, 16);
    BOOST_CHECK_EQUAL(memcmp(buffer, out, size), 0);

    value = HexFromReverseBin(buffer, sizeof(buffer));
    BOOST_CHECK_EQUAL(value, "f0efdecdbcab9a897867564534231201");
    size = HexToReverseBin(value.c_str(), out, 16);
    BOOST_CHECK_EQUAL(size, 16);
    BOOST_CHECK_EQUAL(memcmp(buffer, out, size), 0);

    size = HexToBin("12345", out, 16);
    value = HexFromBin(out, size);
    BOOST_CHECK_EQUAL(value, "1234");
    BOOST_CHECK_EQUAL(size, 2);

    size = HexToReverseBin("012345", out, 16);
    value = HexFromReverseBin(out, size);
    BOOST_CHECK_EQUAL(value, "012345");
    BOOST_CHECK_EQUAL(size, 3);

}


BOOST_AUTO_TEST_SUITE_END()
