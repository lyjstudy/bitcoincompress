#include <compress/amount.h>
#include <boost/test/unit_test.hpp>

using namespace compress;

BOOST_AUTO_TEST_SUITE(amount_tests)

BOOST_AUTO_TEST_CASE(base) {
    struct {
        int64_t amount;
        uint64_t compressed;
    } testData[] = {
        {0, 0},
        {10, 10 << 1},
        {100, 0x25},
        {1000, 0x27},
        {10000, 0x29},
        {99999, 99999 << 1},
        {100000, 0x2B},
        {1000000, 0x2D},
        {999000000, 0x7CED},
        {Amount::MAX_VALUE, 0x2BD},
    };

    for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
        auto compressed = Amount::Compress(testData[i].amount);
        auto uncompressed = Amount::Decompress(compressed);
        BOOST_CHECK_EQUAL(compressed, testData[i].compressed);
        BOOST_CHECK_EQUAL(uncompressed, testData[i].amount);
    }
}

BOOST_AUTO_TEST_SUITE_END()
