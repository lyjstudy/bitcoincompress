#include <bkbase/hash.h>
#include <crypto/hasher.h>
#include <bkbase/logging.h>
#include <bkbase/hex.h>

#include <boost/test/unit_test.hpp>



BOOST_AUTO_TEST_SUITE(hash_tests)

BOOST_AUTO_TEST_CASE(logic) {
    crypto::CHash160 hash160;
    hash160.Write(bkbase::HexToBin("04bcac2a17441518d8e0dac8084973237a13728251646a88418c15e2178ddf238e924a1e473221beadacaa3875c726c78f8e02d80962a94bb5e451d52b5d80fbdb"));
    BOOST_CHECK_EQUAL(std::string("6ccf92d90a461a99a934106de9c9a7cc58038358"), hash160.Finalize().GetHex());
}

BOOST_AUTO_TEST_SUITE_END()
