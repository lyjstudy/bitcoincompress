#include <compress/pubkey.h>
#include <boost/test/unit_test.hpp>
#include <string>
#include <bkbase/hex.h>

using namespace compress;

BOOST_AUTO_TEST_SUITE(pubkey_tests)

BOOST_AUTO_TEST_CASE(base) {

    std::string goodPubkey[] = {
        "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f",
        "0496b538e853519c726a2c91e61ec11600ae1390813a627c66fb8be7947be63c52da7589379515d4e0a604f8141781e62294721166bf621e73a82cbf2342c858ee",
    };

    for (size_t i = 0; i < sizeof(goodPubkey) / sizeof(goodPubkey[0]); i++) {

        auto pubkey = bkbase::HexToBin(goodPubkey[i]);

        std::vector<uint8_t> compress;
        BOOST_CHECK(PubKey::Compress(pubkey, compress));
        std::vector<uint8_t> uncompress;
        BOOST_CHECK(PubKey::Decompress(compress, uncompress));

        BOOST_CHECK(compress.size() < uncompress.size());
        BOOST_CHECK_EQUAL(bkbase::HexFromBin(pubkey), bkbase::HexFromBin(uncompress));

    }

    std::vector<uint8_t> data = bkbase::HexToBin("02118a7dfa0131348ea33b26090acd074cbb5f7ef46877a45afa25f8761a9620e7");
    
    std::vector<uint8_t> uncompress;
    BOOST_CHECK(PubKey::Decompress(data, uncompress));
    std::vector<uint8_t> compress;
    BOOST_CHECK(PubKey::Compress(uncompress, compress));
    BOOST_CHECK(compress.size() < uncompress.size());
    BOOST_CHECK_EQUAL(bkbase::HexFromBin(compress), bkbase::HexFromBin(data));
}

BOOST_AUTO_TEST_SUITE_END()
