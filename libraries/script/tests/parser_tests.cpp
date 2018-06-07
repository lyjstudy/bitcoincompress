
#define BOOST_TEST_MAIN
#include "script/parser.h"
#include "script/type.h"

#include <string.h>
#include <boost/test/unit_test.hpp>

using namespace core;

bool CompareVector(const std::vector<uint8_t> &v1, const std::vector<uint8_t> &v2) {
    if (v1.size() != v2.size()) return false;
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
        // PubKey
        {
            script::Type::PubKey,
            {
                40, // PUSH(40)
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 0,
                OP_CHECKSIG,
            },
            {
                40, OP_CHECKSIG,
            },
            {
                {
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 0,
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
                40,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 0,
                40,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 1,
                40,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 0, 0,  0, 0, 0, 2,
                OP_3, OP_CHECKMULTISIG,
            },
            {
                OP_2, 40, 40, 40, OP_3, OP_CHECKMULTISIG,
            },
            {
                {
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 0,
                },
                {
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 1,
                },
                {
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
                    0, 0, 0, 0,  0, 0, 0, 2,
                },
            },
        },
    };
    
    for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
        auto &script = testData[i].script;
        auto &temp = testData[i].temp;
        auto &data = testData[i].data;

        std::vector<uint8_t> outTemp;
        script::Parser::GetScriptTemplate(script, outTemp);
        BOOST_CHECK(CompareVector(outTemp, temp));

        std::vector<std::vector<uint8_t>> outData;
        script::Parser::GetScriptTemplate(script, outTemp, &outData);
        BOOST_CHECK(CompareVector(outTemp, temp));
        BOOST_CHECK(CompareVector(outData, data));

        BOOST_CHECK(script::GetOutputType(script) == testData[i].type);

        printf("%s\n", GetOpString(testData[i].script).c_str());
    }


}

BOOST_AUTO_TEST_SUITE_END()
