#include <script/type.h>
#include <script/parser.h>
#include <script/signature.h>

namespace script {

    static bool IsSignatureCompressable(const uint8_t *sig, size_t size) {
        if (size < 9 || size > 73) return false;
        if (sig[0] != 0x30) return false;
        if (sig[1] != size - 3) return false;
        if (sig[2] != 0x02) return false;

        auto lenR = sig[3];
        if (5 + lenR >= size) return false;
        auto lenS = sig[5 + lenR];
        if (lenR + lenS + 7 != size) return false;
        auto sighash = sig[size - 1];

        // sighash use 0 1 2 6 7 bits
        if ((sighash & 0x38) != 0) return false;

        // lenR use 3 4 5bits (29 -> 36)
        if (lenR < 29 || lenR > 36) return false;
        return true;
    }

    bool IsInputPubKey(const std::vector<uint8_t> &script) {
        if (script.empty()) return false;
        if ((size_t)script[0] + 1 != script.size()) return false;
        
        return IsSignatureCompressable(&script[1], script.size() - 1);
    }

    bool IsInputPubKeyHash(const std::vector<uint8_t> &script) {
        if (script.empty()) return false;
        // PUSH signature PUSH pubkey
    }

    const static uint8_t PubKeyHashTemplate[] = {
        OP_DUP, OP_HASH160, 20, OP_EQUALVERIFY, OP_CHECKSIG,
    };
    const static uint8_t ScriptHashTemplate[] = {
        OP_HASH160, 20, OP_EQUAL,
    };

    Type GetOutputType(const std::vector<uint8_t> &script) {
        if (script.empty()) return Type::NonStandard;

        std::vector<uint8_t> scriptTemplate;
        script::Parser::GetScriptTemplate(script, scriptTemplate);
        if (scriptTemplate.empty()) return Type::NonStandard;

        uint8_t op = scriptTemplate[0];

        // PubKey: PUSH(33=>65) OP_CHECKSIG
        if (op >= 33 && op <= 65 && scriptTemplate.size() == 2 && scriptTemplate[1] == OP_CHECKSIG) {
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
            bool isOpReturn = true;
            for (size_t i = 1; i < scriptTemplate.size(); i++) {
                if (scriptTemplate[i] > OP_16) {
                    isOpReturn = false; // PushOnly
                    break;
                }
            }
            if (isOpReturn) return Type::OpReturn;
        }
        // MultiSig: n(1=>3) PUSH(33=>65)... m(n=>16) OP_CHECKMULTISIG
        if (op >= OP_1 && op <= OP_3) {
            int n = (int)op - (int)OP_1 + 1;
            if (scriptTemplate.size() >= (size_t)n + 3 && scriptTemplate.back() == OP_CHECKMULTISIG) {
                int m = (int)scriptTemplate[scriptTemplate.size() - 2] - (int)OP_1 + 1;
                if (m >= n && m <= 16 && scriptTemplate.size() == (size_t)m + 3) {
                    bool isValidMultiSig = true;
                    for (int i = 1; i <= m; i++) {
                        if (scriptTemplate[i] < 33 || scriptTemplate[i] > 65) {
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