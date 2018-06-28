#pragma once

#include <stdint.h>
#include <vector>
#include <string>

#define OP_CODE_FOR_EACH(T) \
T(0,                    0x00) \
T(PUSHDATA1,            0x4c) \
T(PUSHDATA2,            0x4d) \
T(PUSHDATA4,            0x4e) \
T(1NEGATE,              0x4f) \
T(RESERVED,             0x50) \
T(1,                    0x51) \
T(2,                    0x52) \
T(3,                    0x53) \
T(4,                    0x54) \
T(5,                    0x55) \
T(6,                    0x56) \
T(7,                    0x57) \
T(8,                    0x58) \
T(9,                    0x59) \
T(10,                   0x5A) \
T(11,                   0x5B) \
T(12,                   0x5C) \
T(13,                   0x5D) \
T(14,                   0x5E) \
T(15,                   0x5F) \
T(16,                   0x60) \
T(NOP,                  0x61) \
T(VER,                  0x62) \
T(IF,                   0x63) \
T(NOTIF,                0x64) \
T(VERIF,                0x65) \
T(VERNOTIF,             0x66) \
T(ELSE,                 0x67) \
T(ENDIF,                0x68) \
T(VERIFY,               0x69) \
T(RETURN,               0x6A) \
T(TOALTSTACK,           0x6B) \
T(FROMALTSTACK,         0x6C) \
T(2DROP,                0x6D) \
T(2DUP,                 0x6E) \
T(3DUP,                 0x6F) \
T(2OVER,                0x70) \
T(2ROT,                 0x71) \
T(2SWAP,                0x72) \
T(IFDUP,                0x73) \
T(DEPTH,                0x74) \
T(DROP,                 0x75) \
T(DUP,                  0x76) \
T(NIP,                  0x77) \
T(OVER,                 0x78) \
T(PICK,                 0x79) \
T(ROLL,                 0x7A) \
T(ROT,                  0x7B) \
T(SWAP,                 0x7C) \
T(TUCK,                 0x7D) \
T(CAT,                  0x7E) \
T(SPLIT,                0x7F) \
T(NUM2BIN,              0x80) \
T(BIN2NUM,              0x81) \
T(SIZE,                 0x82) \
T(INVERT,               0x83) \
T(AND,                  0x84) \
T(OR,                   0x85) \
T(XOR,                  0x86) \
T(EQUAL,                0x87) \
T(EQUALVERIFY,          0x88) \
T(RESERVED1,            0x89) \
T(RESERVED2,            0x8A) \
T(1ADD,                 0x8B) \
T(1SUB,                 0x8C) \
T(2MUL,                 0x8D) \
T(2DIV,                 0x8E) \
T(NEGATE,               0x8F) \
T(ABS,                  0x90) \
T(NOT,                  0x91) \
T(0NOTEQUAL,            0x92) \
T(ADD,                  0x93) \
T(SUB,                  0x94) \
T(MUL,                  0x95) \
T(DIV,                  0x96) \
T(MOD,                  0x97) \
T(LSHIFT,               0x98) \
T(RSHIFT,               0x99) \
T(BOOLAND,              0x9A) \
T(BOOLOR,               0x9B) \
T(NUMEQUAL,             0x9C) \
T(NUMEQUALVERIFY,       0x9D) \
T(NUMNOTEQUAL,          0x9E) \
T(LESSTHAN,             0x9F) \
T(GREATERTHAN,          0xA0) \
T(LESSTHANOREQUAL,      0xA1) \
T(GREATERTHANOREQUAL,   0xA2) \
T(MIN,                  0xA3) \
T(MAX,                  0xA4) \
T(WITHIN,               0xA5) \
T(RIPEMD160,            0xA6) \
T(SHA1,                 0xA7) \
T(SHA256,               0xA8) \
T(HASH160,              0xA9) \
T(HASH256,              0xAA) \
T(CODESEPARATOR,        0xAB) \
T(CHECKSIG,             0xAC) \
T(CHECKSIGVERIFY,       0xAD) \
T(CHECKMULTISIG,        0xAE) \
T(CHECKMULTISIGVERIFY,  0xAF) \
T(NOP1,                 0xB0) \
T(CHECKLOCKTIMEVERIFY,  0xB1) \
T(CHECKSEQUENCEVERIFY,  0xB2) \
T(NOP4,                 0xB3) \
T(NOP5,                 0xB4) \
T(NOP6,                 0xB5) \
T(NOP7,                 0xB6) \
T(NOP8,                 0xB7) \
T(NOP9,                 0xB8) \
T(NOP10,                0xB9)

#define OP_CODE_ALIAS_FOR_EACH(T) \
T(FALSE,                0) \
T(TRUE,                 1) \
T(NOP2,                 CHECKLOCKTIMEVERIFY) \
T(NOP3,                 CHECKSEQUENCEVERIFY)


constexpr uint8_t OP_INVALIDOPCODE = 0xFF;
#define DEFINE_OP_CODE(name, value) constexpr uint8_t OP_##name = value;
OP_CODE_FOR_EACH(DEFINE_OP_CODE)
#undef DEFINE_OP_CODE
#define DEFINE_OP_CODE(name, value) constexpr uint8_t OP_##name = OP_##value;
OP_CODE_ALIAS_FOR_EACH(DEFINE_OP_CODE)
#undef DEFINE_OP_CODE


namespace script {

const char *GetOpName(uint8_t opcode);

uint8_t GetOpValue(const std::string &name);

std::string DisAssemble(const std::vector<uint8_t> &codes);

// -1  OK
// >=0 Error Position
std::vector<uint8_t> Assemble(const std::string &codeStr);

} // script
