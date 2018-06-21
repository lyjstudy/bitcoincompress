#pragma once

#include <stdint.h>
#include <vector>
#include <bkbase/serialize.h>

namespace compress {

enum class ScriptType {
    Binary, // native script
    P2PK, // input: signature                                  output: pubkeyIndex
    P2PKH, // input: signature                                 output: pubkeyIndex
    MS, // input: signature1...                                output: n, m, pubkeyIndex1...
    P2SHP2PK, // input: signature                              output: pubkeyIndex
    P2SHP2PKH, // input: signature                             output: pubkeyIndex
    P2SHMS, // input: signature1...                            output: n, m, pubkeyIndex1...
};

class TransactionInput {
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation serAction) {
        //READWRITE(mBlockHeight);
        //READWRITE(mTransactionIndex);
        //READWRITE(mTransactionInIndex);
        READWRITE(mSequence);
        READWRITE(mScripType);
        READWRITE(mUnlockScript);
    }
protected:
    // bkbase::Hash256 mTxInID;
    uint32_t mBlockHeight;
    uint32_t mTransactionIndex;
    // uint32_t mTxInIndex;
    uint32_t mTransactionInIndex;
    // uint32_t mSequence;
    uint32_t mSequence;
    // std::vector<uint8_t> mUnlockScript;
    uint8_t mScripType;
    std::vector<uint8_t> mUnlockScript;
};

class TransactionOutput {
protected:
    // int64_t mAmount;
    uint64_t mAmount;
    // std::vector<uint8_t> mLockScript;
    ScriptType mScripType;
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