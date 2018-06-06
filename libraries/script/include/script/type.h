#pragma once

#include <stdint.h>
#include <vector>
#include "opcode.h"

namespace script {

    enum class Type {
        NonStandard,

        // lock: PUSH(33=>65) OP_CHECKSIG
        PubKey,

        // lock: OP_DUP OP_HASH160 PUSH(20) OP_EQUALVERIFY OP_CHECKSIG
        PubKeyHash,

        // lock: OP_HASH160 PUSH(20) OP_EQUAL
        ScriptHash,

        // lock: n(1=>3) PUSH(33=>65)... m(n=>16) OP_CHECKMULTISIG
        MultiSig,

        // lock: OP_RETURN ...(PUSH ONLY)
        // unlock: NO UNLOCK
        OpReturn,
    };

    Type GetOutputType(const std::vector<uint8_t> &script);

}