#pragma once

#include <stdint.h>
#include <vector>
#include "opcode.h"

namespace script {

    enum class Type {
        BadScript,
        NonStandard,

        // lock: PUSH(33=>65) OP_CHECKSIG
        // unlock: PUSH(signature)
        PubKey,

        // lock: OP_DUP OP_HASH160 PUSH(20) OP_EQUALVERIFY OP_CHECKSIG
        // unlock: PUSH(signature) PUSH(33=>65)
        PubKeyHash,

        // lock: OP_HASH160 PUSH(20) OP_EQUAL
        // unlock_PubKey: PUSH(signature) PUSH(pubkeyscript(pubkey))
        // unlock_PubKeyHash: PUSH(signature) PUSH(pubkey) PUSH(pubkeyhashscripts(pubkey))
        // unlock_MultiSig: OP_0 PUSH(signature1) ... PUSH(multisigscript(n, m, pubkey1, ...))
        ScriptHash,

        // lock: n(1=>3) PUSH(33=>65)... m(n=>16) OP_CHECKMULTISIG
        // unlock: OP_0 PUSH(signature1)...
        MultiSig,

        // lock: OP_RETURN ...(PUSH ONLY)
        // unlock: NO UNLOCK
        OpReturn,

        // Input Only
        PubKeyBadInput,
        PubKeyHashBadInput,
        MultiSigBadInput,
    };

    /*
        PubKey: info = { pubkey },
        PubKeyHash: info = { pubkeyhash },
        ScriptHash: info = { scripthash },
        MultiSig: info = { n, m, pubkey1, ... },
        OpReturn: info = {},
        BadScript: info = {},
        NonStandard: info = {},
    */
    Type GetOutputType(const std::vector<uint8_t> &script, std::vector<std::vector<uint8_t>> *info = nullptr);

    /*
        PubKey: info = { signature },
        PubKeyHash: info = { signature, pubkey },
        MultiSig: info = { sig1, ... },
        OpReturn: info = {},
        BadScript: info = {},
        NonStandard: info = {},
        ScriptHash_PubKey: info = { pubkey, signature },
        ScriptHash_PubKeyHash: info = { pubkeyhash, signature, pubkey },
        ScriptHash_MultiSig: info = { n, m, pubkey1, ..., sig1, ...},
    */
    Type GetInputType(const std::vector<uint8_t> &script, Type output, std::vector<std::vector<uint8_t>> *info = nullptr);

}
