#include <bkbase/exception.h>
#include <bkbase/formats.h>
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>

BOOST_AUTO_TEST_SUITE(formats_tests)

BOOST_AUTO_TEST_CASE(base) {
    BOOST_CHECK(bkbase::Digit2.IsValidChar('0'));
    BOOST_CHECK(bkbase::Digit2.IsValidChar('1'));
    BOOST_CHECK(!bkbase::Digit2.IsValidChar('2'));
    BOOST_CHECK(bkbase::Digit2.IsValidString("01010111010101110"));

    BOOST_CHECK_EQUAL(bkbase::Digit2.EncodeSize(10), 80);
    BOOST_CHECK_EQUAL(bkbase::Digit2.DecodeSize(80), 10);

    BOOST_CHECK_EQUAL(bkbase::Digit2.Encode({3}), "00000011");
    BOOST_CHECK_EQUAL(bkbase::Digit2.Encode({3, 3}), "0000001100000011");
    BOOST_CHECK_EQUAL(bkbase::Digit2.Encode({3, 4, 5}), "000001010000010000000011");
    BOOST_CHECK_EQUAL(bkbase::Digit16.Encode({3, 4, 5}), "050403");

    BOOST_CHECK_EQUAL(bkbase::Digit16.Encode<uint8_t>(0xab), "AB");
    BOOST_CHECK_EQUAL(bkbase::Digit16.Encode<uint16_t>(0x0102), "0201");
    BOOST_CHECK_EQUAL(bkbase::Digit16.Encode<uint32_t>(0x01020304), "04030201");
    BOOST_CHECK_EQUAL(bkbase::Digit16.Encode<uint64_t>(0x0102030405060708), "0807060504030201");
}

BOOST_AUTO_TEST_SUITE_END()
