#include "script/parser.h"
#include "script/type.h"

#include <algorithm>
#include <string.h>
#include <boost/test/unit_test.hpp>

bool CompareVector(const std::vector<uint8_t> &v1, const std::vector<uint8_t> &v2) {
    if (v1.size() != v2.size()) return false;
    if (v1.empty()) return true;
    return memcmp(&v1[0], &v2[0], v1.size()) == 0;
}

bool CompareVector(const std::vector<std::vector<uint8_t>> &v1, const std::vector<std::vector<uint8_t>> &v2) {
    if (v1.size() != v2.size()) return false;
    for (size_t i = 0; i < v1.size(); i++) {
        if (!CompareVector(v1[i], v2[i])) return false;
    }
    return true;
}

BOOST_AUTO_TEST_SUITE(type_tests)

BOOST_AUTO_TEST_CASE(simple) {

    struct {
        script::Type         type;
        std::vector<uint8_t> script;
    } testData[] = {
        // OpReturn
        {
            script::Type::OpReturn,
            {
                OP_RETURN, OP_16,
            },
        },
        // BadScript
        {
            script::Type::BadScript,
            {
                OP_DUP, OP_16, 22, OP_8,
            },
        },
        // NonStandard
        {
            script::Type::NonStandard,
            {
                1, OP_0
            },
        },
    };
    
    for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
        auto &script = testData[i].script;

        BOOST_CHECK_EQUAL((int)script::GetOutputType(script), (int)testData[i].type);
    }


}

BOOST_AUTO_TEST_SUITE_END()
