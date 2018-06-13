#include <compress/pubkeydb.h>
#include <boost/test/unit_test.hpp>
#include <string>
#include <bkbase/hex.h>
#include <boost/filesystem.hpp>
#include <iostream>

using namespace compress;

namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(pubkeydb_tests)

BOOST_AUTO_TEST_CASE(base) {
    auto tempDB = bfs::temp_directory_path() / "bitcoincompress_pubkeydb";
    PubKeyDB db;
    BOOST_CHECK(db.Open(tempDB.string().c_str()));

    auto pub1 = bkbase::HexToBin("033e01efbe5689e568beabc1630511414a7b4f7a79003421c147ed18a5a75a40e4");
    auto pub2 = bkbase::HexToBin("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f");

    BOOST_CHECK_NE(db.Add(pub1), INVALID_INDEX);
    BOOST_CHECK_NE(db.Add(pub2), INVALID_INDEX);
}

BOOST_AUTO_TEST_SUITE_END()
