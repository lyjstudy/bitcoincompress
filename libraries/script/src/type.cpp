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

    static Type _GetInputType(Type output, const std::vector<uint8_t> &scriptTemplate,
                        const std::vector<std::vector<uint8_t>> &stack, std::vector<std::vector<uint8_t>> *addrs) {
        switch (output) {
            case Type::PubKey:
                if (scriptTemplate.size() == 1 && stack.size() == 1) {
                    if (addrs != nullptr) *addrs = std::move(stack);
                    return Type::PubKey;
                }
                break;
            case Type::PubKeyHash:
                if (scriptTemplate.size() == 2 && stack.size() == 2) {
                    if (addrs != nullptr) *addrs = std::move(stack);
                    return Type::PubKeyHash;
                }
                break;
            case Type::MultiSig:
                if (scriptTemplate.size() > 1 && scriptTemplate[0] == OP_0 && stack.size() == scriptTemplate.size()) {
                    if (addrs != nullptr) {
                        *addrs = std::move(stack);
                        addrs->erase(addrs->begin());
                    }
                    return Type::MultiSig;
                }
                break;
            default:
                break;
        }
        return Type::NonStandard;
    }

    Type GetInputType(const std::vector<uint8_t> &script, Type output, std::vector<std::vector<uint8_t>> *addrs) {
        if (addrs != nullptr) addrs->clear();

        if (output == Type::BadScript) return Type::BadScript;

        if (script.empty()) return Type::NonStandard;
        std::vector<uint8_t> scriptTemplate;
        std::vector<std::vector<uint8_t>> stack;
        if (script::Parser::GetTemplate(script, scriptTemplate, &stack) != 0)
            return Type::BadScript;
        if (scriptTemplate.empty()) return Type::BadScript;

        switch (output) {
            case Type::ScriptHash:
                if (stack.empty() || scriptTemplate.empty()) {
                    break;
                } else {
                    auto type = GetOutputType(stack.back(), addrs);
                    stack.pop_back();
                    scriptTemplate.pop_back();
                    switch (type) {
                        case Type::PubKey:
                            if (_GetInputType(type, scriptTemplate, stack, addrs) != type) {
                                if (addrs != nullptr) addrs->clear();
                                return Type::PubKeyBadInput;
                            }
                            return type;
                        case Type::PubKeyHash:
                            if (_GetInputType(type, scriptTemplate, stack, addrs) != type) {
                                if (addrs != nullptr) addrs->clear();
                                return Type::PubKeyHashBadInput;
                            }
                            return type;
                        case Type::MultiSig:
                            if (_GetInputType(type, scriptTemplate, stack, addrs) != type) {
                                if (addrs != nullptr) addrs->clear();
                                return Type::MultiSigBadInput;
                            }
                            return type;
                        default:
                            break;
                    }
                    Type t = _GetInputType(output, scriptTemplate, stack, addrs);
                    if (t != Type::NonStandard)
                        return t;
                }
                break;
            default: {
                    Type t = _GetInputType(output, scriptTemplate, stack, addrs);
                    if (t != Type::NonStandard)
                        return t;
                }
                break;
        }
        if (addrs != nullptr) addrs->clear();
        return Type::NonStandard;
    }

    Type GetOutputType(const std::vector<uint8_t> &script, std::vector<std::vector<uint8_t>> *addrs) {
        if (addrs != nullptr) addrs->clear();

        if (script.empty()) return Type::BadScript;
        std::vector<uint8_t> scriptTemplate;
        std::vector<std::vector<uint8_t>> stack;
        if (script::Parser::GetTemplate(script, scriptTemplate, &stack) != 0)
            return Type::BadScript;
        if (scriptTemplate.empty()) return Type::BadScript;

        uint8_t op = scriptTemplate[0];

        // PubKey: PUSH(33, 65) OP_CHECKSIG
        if ((op == 33 || op == 65) && scriptTemplate.size() == 2 && scriptTemplate[1] == OP_CHECKSIG) {
            if (addrs != nullptr) addrs->push_back(std::move(stack.back()));
            return Type::PubKey;
        }
        // PubKeyHash: OP_DUP OP_HASH160 PUSH(20) OP_EQUALVERIFY OP_CHECKSIG
        if (scriptTemplate.size() == 5 && memcmp(&scriptTemplate[0], PubKeyHashTemplate, 5) == 0) {
            if (addrs != nullptr) addrs->push_back(std::move(stack.back()));
            return Type::PubKeyHash;
        }
        // ScriptHash: OP_HASH160 PUSH(20) OP_EQUAL
        if (scriptTemplate.size() == 3 && memcmp(&scriptTemplate[0], ScriptHashTemplate, 3) == 0) {
            if (addrs != nullptr) addrs->push_back(std::move(stack.back()));
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
        // MultiSig: n(1=>16) PUSH(33, 65)... m(n=>16) OP_CHECKMULTISIG
        if (op >= OP_1 && op <= OP_16) {
            int n = (int)op - (int)OP_1 + 1;
            if (scriptTemplate.size() >= (size_t)n + 3 && scriptTemplate.back() == OP_CHECKMULTISIG) {
                int m = (int)scriptTemplate[scriptTemplate.size() - 2] - (int)OP_1 + 1;
                if (m >= n && m <= 16 && scriptTemplate.size() == (size_t)m + 3) {
                    bool isValidMultiSig = true;
                    if (addrs != nullptr) addrs->push_back({ (uint8_t)n });
                    if (addrs != nullptr) addrs->push_back({ (uint8_t)m });
                    for (int i = 1; i <= m; i++) {
                        if (scriptTemplate[i] != 33 && scriptTemplate[i] != 65) {
                            isValidMultiSig = false;
                            break;
                        }
                        if (addrs != nullptr) addrs->push_back(stack[i]);
                    }
                    if (isValidMultiSig) {
                        return Type::MultiSig;
                    }
                }
            }
        }
        if (addrs != nullptr) addrs->clear();

        return Type::NonStandard;
    }
}