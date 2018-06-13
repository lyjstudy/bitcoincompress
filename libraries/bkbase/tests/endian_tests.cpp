#include <bkbase/endian.h>
#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE(endian_tests)

BOOST_AUTO_TEST_CASE(readwrite) {
    #define IMPL_TEST_READWRITE(type, value, endian) \
        BOOST_CHECK_EQUAL(bkbase::Read##endian<type>(buffer), (type)value); \
        memset(data, 0, sizeof(data)); \
        bkbase::Write##endian<type>(data, (type)value); \
        BOOST_CHECK_EQUAL(memcmp(buffer, data, sizeof(type)), 0);

    uint8_t buffer1[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t *buffer = buffer1;
    uint8_t data[8];
    
    IMPL_TEST_READWRITE(uint8_t, 0x01, BE);
    IMPL_TEST_READWRITE(int8_t, 0x01, BE);
    IMPL_TEST_READWRITE(uint16_t, 0x0102, BE);
    IMPL_TEST_READWRITE(int16_t, 0x0102, BE);
    IMPL_TEST_READWRITE(uint32_t, 0x01020304, BE);
    IMPL_TEST_READWRITE(int32_t, 0x01020304, BE);
    IMPL_TEST_READWRITE(uint64_t, 0x0102030405060708L, BE);
    IMPL_TEST_READWRITE(int64_t, 0x0102030405060708L, BE);
    IMPL_TEST_READWRITE(float, 2.38793926e-38f, BE);
    IMPL_TEST_READWRITE(double, 8.2078803991318393e-304, BE);

    IMPL_TEST_READWRITE(uint8_t, 0x01, LE);
    IMPL_TEST_READWRITE(int8_t, 0x01, LE);
    IMPL_TEST_READWRITE(uint16_t, 0x0201, LE);
    IMPL_TEST_READWRITE(int16_t, 0x0201, LE);
    IMPL_TEST_READWRITE(uint32_t, 0x04030201, LE);
    IMPL_TEST_READWRITE(int32_t, 0x04030201, LE);
    IMPL_TEST_READWRITE(uint64_t, 0x0807060504030201L, LE);
    IMPL_TEST_READWRITE(int64_t, 0x0807060504030201L, LE);
    IMPL_TEST_READWRITE(float, 1.53998961e-36f, LE);
    IMPL_TEST_READWRITE(double, 5.447603722011605e-270, LE);
    
    uint8_t buffer2[] = {0xFF, 0x02, 0x03, 0xFF, 0x05, 0x06, 0x07, 0xFF};
    buffer = buffer2;
    IMPL_TEST_READWRITE(int8_t, 0xFF, BE);
    IMPL_TEST_READWRITE(int8_t, 0xFF, LE);
    IMPL_TEST_READWRITE(int16_t, 0xFF02, BE);
    IMPL_TEST_READWRITE(int16_t, 0x02FF, LE);
    IMPL_TEST_READWRITE(int32_t, 0xFF0203FF, BE);
    IMPL_TEST_READWRITE(int32_t, 0xFF0302FF, LE);
    IMPL_TEST_READWRITE(int64_t, 0xFF0203FF050607FF, BE);
    IMPL_TEST_READWRITE(int64_t, 0xFF070605FF0302FF, LE);
}

BOOST_AUTO_TEST_SUITE_END()
