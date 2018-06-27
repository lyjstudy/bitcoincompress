#include <compress/signature.h>
#include <boost/test/unit_test.hpp>
#include <string>
#include <bkbase/hex.h>

using namespace compress;

BOOST_AUTO_TEST_SUITE(signature_tests)

BOOST_AUTO_TEST_CASE(base) {

    std::string goodSigs[] = {
        "3045022100a690f1f9a675a857901bda77f133a11ed5f4254f89c0a8e09a3a07ecd579fdff02205d2ca4aff10b45f1d189953ca4599fbe3a72f165c96098dfc287d62f1c7f07b401",
        "304402201cd5a36fbc97f979ec0573e25def8fa18c5e849095a068d1c637d4f6eb50dca60220566c2c6ba9ba2794ebd92cf77499fce4c1a4c915dd92f87877b6483a86e3946001",
        "3045022100f036fb38dfac8f58fd211ef6e2cd658e7066516637ac73e741b4e7205039a675022043244c31a1b4023c48f2f48dab9645ee241bf2bddf9bce9efb2fc435ff0e264c01",
        "3045022100b7119928ab6cad4ccf41fd25d0a79b5779a30e2b05ede826ef22bf8e68efcbf002205f22d16128d0a7e79e5ecc8d907025803895858a4fe35bab2e4dbe8e9d2345ca01",
    };

    for (size_t i = 0; i < sizeof(goodSigs) / sizeof(goodSigs[0]); i++) {

        auto sig = bkbase::HexToBin(goodSigs[i]);

        BOOST_CHECK(Signature::IsSupport(sig));

        auto compress = Signature::Compress(sig);
        auto uncompress = Signature::Decompress(compress);

        BOOST_CHECK(compress.size() < uncompress.size());
        BOOST_CHECK_EQUAL(bkbase::HexFromBin(sig), bkbase::HexFromBin(uncompress));

    }
}

BOOST_AUTO_TEST_SUITE_END()
