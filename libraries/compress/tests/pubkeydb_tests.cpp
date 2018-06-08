#include <compress/pubkeydb.h>
#include <boost/test/unit_test.hpp>
#include <string>
#include <hex.h>
#include <boost/filesystem.hpp>
#include <iostream>

using namespace compress;

namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(pubkeydb_tests)

BOOST_AUTO_TEST_CASE(base) {
    auto tempDB = bfs::temp_directory_path() / "test";
    PubKeyDB db;
    BOOST_CHECK(db.Open(tempDB.string().c_str()));
}

BOOST_AUTO_TEST_SUITE_END()
