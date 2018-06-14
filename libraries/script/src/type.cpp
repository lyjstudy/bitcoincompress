#include <algorithm>
#include <functional>
#include <script/type.h>
#include <script/parser.h>

namespace script {

    const static uint8_t PubKeyHashTemplate[] = {
        OP_DUP, OP_HASH160, 20, OP_EQUALVERIFY, OP_CHECKSIG,
    };
    const static uint8_t ScriptHashTemplate[] = {
        OP_HASH160, 20, OP_EQUAL,
    };

    Type GetOutputType(const std::vector<uint8_t> &script) {
        if (script.empty()) return Type::NonStandard;

        std::vector<uint8_t> scriptTemplate;
        if (script::Parser::GetTemplate(script, scriptTemplate) != 0)
            return Type::NonStandard;
        if (scriptTemplate.empty()) return Type::NonStandard;

        uint8_t op = scriptTemplate[0];

        // PubKey: PUSH(33, 65) OP_CHECKSIG
        if ((op == 33 || op == 65) && scriptTemplate.size() == 2 && scriptTemplate[1] == OP_CHECKSIG) {
            return Type::PubKey;
        }
        // PubKeyHash: OP_DUP OP_HASH160 PUSH(20) OP_EQUALVERIFY OP_CHECKSIG
        if (scriptTemplate.size() == 5 && memcmp(&scriptTemplate[0], PubKeyHashTemplate, 5) == 0) {
            return Type::PubKeyHash;
        }
        // ScriptHash: OP_HASH160 PUSH(20) OP_EQUAL
        if (scriptTemplate.size() == 3 && memcmp(&scriptTemplate[0], ScriptHashTemplate, 3) == 0) {
            return Type::ScriptHash;
        }
        // OpReturn: OP_RETURN PUSH...
        if (op == OP_RETURN) {
            if (scriptTemplate.size() == 1) return Type::NonStandard;
            if (std::find_if(scriptTemplate.begin() + 1, scriptTemplate.end(), 
                [] (uint8_t val) {
                    return val > OP_16;
                }) != scriptTemplate.end()) {
                return Type::NonStandard;
            }
            return Type::OpReturn;
        }
        // MultiSig: n(1=>3) PUSH(33, 65)... m(n=>16) OP_CHECKMULTISIG
        if (op >= OP_1 && op <= OP_3) {
            int n = (int)op - (int)OP_1 + 1;
            if (scriptTemplate.size() >= (size_t)n + 3 && scriptTemplate.back() == OP_CHECKMULTISIG) {
                int m = (int)scriptTemplate[scriptTemplate.size() - 2] - (int)OP_1 + 1;
                if (m >= n && m <= 16 && scriptTemplate.size() == (size_t)m + 3) {
                    bool isValidMultiSig = true;
                    for (int i = 1; i <= m; i++) {
                        if (scriptTemplate[i] != 33 && scriptTemplate[i] != 65) {
                            isValidMultiSig = false;
                            break;
                        }
                    }
                    if (isValidMultiSig) {
                        return Type::MultiSig;
                    }
                }
            }
        }

        return Type::NonStandard;
    }
}