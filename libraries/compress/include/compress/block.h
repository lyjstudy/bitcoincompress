#pragma once

#include <stdint.h>
#include <vector>
#include <bkbase/serialize.h>
#include <core/block.h>

namespace compress {

enum {
    /*
        未做任何处理
    */
    SCRIPT_NATIVE = 0,

    /*
        普通公钥交易
        输入: PUSH(signature) => signature
        输出: PUSH(pubkey) CHECKSIG => pubkeyIndex
    */
    SCRIPT_P2PK,

    /*
        普通公钥HASH交易
        1. 输入在解压时依赖对应输出
        输入: PUSH(signature) PUSH(pubkey) => signature
        输出: DUP HASH160 PUSH(pubkeyhash) EQUALVERIFY CHECKSIG => pubkeyIndex
    */
    SCRIPT_P2PKH,

    /*
        多重签名交易
        输入: 0 PUSH(signature1)... => signature1...
        输出: n PUSH(pubkey1) ... m CHECKMULTISIG => n m pubkeyIndex1 ...
    */
    SCRIPT_MS,

    /*
        P2SH标准公钥交易
        1. 输入在解压缩时依赖对应的输出
        输入: PUSH(signature) PUSH(PUSH(pubkey) CHECKSIG) => signature
        输出: HASH160 PUSH(scripthash) EQUAL => pubkeyIndex1
    */
    SCRIPT_P2SHP2PK,

    /*
        P2SH标准公钥HASH交易
        1. 输入在解压缩时依赖对应的输出
        输入: PUSH(signature) PUSH(pubkey) PUSH(DUP HASH160 PUSH(pubkeyhash) EQUALVERIFY CHECKSIG) => signature
        输出: HASH160 PUSH(scripthash) EQUAL => pubkeyIndex1
    */
    SCRIPT_P2SHP2PKH,

    /*
        P2SH多重签名交易
        1. 输入在解压缩时依赖对应的输出
        输入: 0 PUSH(signature1)... PUSH(n PUSH(pubkey1) ... m CHECKMULTISIG) => signature1...
        输出: HASH160 PUSH(scripthash) EQUAL => n, m, pubkeyIndex1...
    */
    SCRIPT_P2SHMS,
};

// 压缩时需要 从 txid => height+index 且知道脚本类型
// 解压时需要 从 height+index => txid 且依赖对应的输出
class TransactionInput {
protected:
    // bkbase::Hash256 mTxInID;
    uint32_t mHeight;
    uint32_t mTxIndex;
    // uint32_t mTxInIndex;
    uint32_t mTxInIndex;
    // uint32_t mSequence;
    uint32_t mSequence;
    // std::vector<uint8_t> mUnlockScript;
    uint8_t mScripType;
    std::vector<uint8_t> mUnlockScript;
public:
    
};

class TransactionOutput {
protected:
    // int64_t mAmount;
    uint64_t mAmount;
    // std::vector<uint8_t> mLockScript;
    uint8_t mScripType;
    std::vector<uint8_t> mLockScript;
};

class Transaction {
protected:
    // int32_t mVersion; IN ChangeDB
    // std::vector<TransactionInput> mInputs;
    std::vector<TransactionInput> mInputs;
    // std::vector<TransactionOutput> mOutputs;
    std::vector<TransactionOutput> mOutputs;
    // uint32_t mLockTime;
    uint32_t mLockTime;
};

class Block {
protected:
    // int32_t mVersion; IN ChangeDB
    // uint32_t mBits; IN ChangeDB
    // bkbase::Hash256 mHashPrevBlock; Calc Dynamic
    // bkbase::Hash256 mMerkleRoot; Calc Dynamic
    // uint32_t mTime;
    uint32_t mTimeOffset;
    // uint32_t mNonce;
    uint32_t mNonce;
    // std::vector<TransactionRef> mTransactions;
    std::vector<Transaction> mTransactions;
};

} // compress