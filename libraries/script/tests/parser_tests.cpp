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

BOOST_AUTO_TEST_SUITE(parser_tests)

BOOST_AUTO_TEST_CASE(templa) {

    struct {
        script::Type         type;
        std::vector<uint8_t> script;
        std::vector<uint8_t> temp;
        std::vector<std::vector<uint8_t>>
                             data;
    } testData[] = {
        // OpReturn
        {
            script::Type::OpReturn,
            {
                OP_RETURN, OP_16,
            },
            {
                OP_RETURN, OP_16,
            },
            {
                { 16 },
            },
        },
        // NonStandard
        {
            script::Type::NonStandard,
            {
                OP_RETURN, OP_NOP,
            },
            {
                OP_RETURN, OP_NOP,
            },
            {},
        },
        // NonStandard
        {
            script::Type::NonStandard,
            {
                OP_RETURN,
            },
            {
                OP_RETURN,
            },
            {},
        },
        // PubKey
        {
            script::Type::PubKey,
            {
                33, // PUSH(33)
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0,
                OP_CHECKSIG,
            },
            {
                33, OP_CHECKSIG,
            },
            {
                {
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0,
                },
            },
        },
        // PubKeyHash
        {
            script::Type::PubKeyHash,
            {
                OP_DUP, OP_HASH160, 20,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,
                OP_EQUALVERIFY, OP_CHECKSIG,
            },
            {
                OP_DUP, OP_HASH160, 20, OP_EQUALVERIFY, OP_CHECKSIG,
            },
            {
                {
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,
                },
            },
        },
        // ScriptHash
        {
            script::Type::ScriptHash,
            {
                OP_HASH160, 20,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,
                OP_EQUAL,
            },
            {
                OP_HASH160, 20, OP_EQUAL,
            },
            {
                {
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,
                },
            },
        },
        // MultiSig
        {
            script::Type::MultiSig,
            {
                OP_2, 
                33,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0,
                33,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                1,
                33,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                2,
                OP_3, OP_CHECKMULTISIG,
            },
            {
                OP_2, 33, 33, 33, OP_3, OP_CHECKMULTISIG,
            },
            {
                {
                    2,
                },
                {
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0,
                },
                {
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    1,
                },
                {
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    2,
                },
                {
                    3,
                },
            },
        },
        // NonStandard
        {
            script::Type::NonStandard,
            {
                OP_0,
            },
            {
                OP_0,
            },
            {
                {},
            },
        },
        // NonStandard
        {
            script::Type::NonStandard,
            {
                OP_2,
            },
            {
                OP_2,
            },
            {
                { 2 },
            },
        },
    };
    
    for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
        auto &script = testData[i].script;
        auto &temp = testData[i].temp;
        auto &data = testData[i].data;

        std::vector<uint8_t> outTemp;
        script::Parser::GetTemplate(script, outTemp);
        BOOST_CHECK(CompareVector(outTemp, temp));

        std::vector<std::vector<uint8_t>> outData;
        script::Parser::GetTemplate(script, outTemp, &outData);
        BOOST_CHECK(CompareVector(outTemp, temp));
        BOOST_CHECK(CompareVector(outData, data));

        BOOST_CHECK_EQUAL((int)script::GetOutputType(script), (int)testData[i].type);
    }


}

BOOST_AUTO_TEST_SUITE_END()
