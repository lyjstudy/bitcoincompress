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
    bfs::remove_all(tempDB);
    PubKeyDB db;
    BOOST_CHECK(db.Open(tempDB.string().c_str()));

    auto pubkey = bkbase::HexToBin("04bcac2a17441518d8e0dac8084973237a13728251646a88418c15e2178ddf238e924a1e473221beadacaa3875c726c78f8e02d80962a94bb5e451d52b5d80fbdb");
    auto pubkeyhash = bkbase::HexToBin("6ccf92d90a461a99a934106de9c9a7cc58038358");

    auto pubkeyIndex = db.Add(pubkey);
    BOOST_CHECK(pubkeyIndex != INVALID_INDEX);

    // db.Close();
    // BOOST_CHECK(db.Open(tempDB.string().c_str()));

    BOOST_CHECK(db.Find(pubkeyhash) != INVALID_INDEX);
}

BOOST_AUTO_TEST_SUITE_END()
