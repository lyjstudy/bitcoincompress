#pragma once

#include <stdint.h>
#include <vector>

namespace compress {

/*
    P2PK   L: OP_INVALIDOPCODE OP_CHECKSIG
           U: OP_INVALIDOPCODE
    P2PKH: L: OP_DUP OP_HASH160 OP_INVALIDOPCODE OP_EQUALVERIFY OP_CHECKSIG
           U: OP_INVALIDOPCODE OP_INVALIDOPCODE
    MS:    L: OP_INVALIDOPCODE OP_INVALIDOPCODE ... OP_CHECKMULTISIG
           U: OP_0 OP_INVALIDOPCODE
    P2SH:  L: OP_HASH160 OP_INVALIDOPCODE OP_EQUAL
*/

bool ParseScript(const std::vector<uint8_t> &script, std::vector<uint8_t> &temp, std::vector<std::vector<uint8_t>> &dataArea);

bool GenScript(std::vector<uint8_t> &script, const std::vector<uint8_t> &temp, const std::vector<std::vector<uint8_t>> &dataArea);

} // compress
