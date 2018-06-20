#pragma once

#include <stdint.h>
#include <vector>

namespace compress {

class TransactionInput {
protected:
    uint32_t mBlockHeight;
    uint32_t mTransactionIndex;
    uint32_t mTransactionInIndex;
    uint32_t mSequence;
    uint64_t mUnlockScript;
};

class TransactionOutput {
protected:
    uint64_t mAmount;
    uint64_t mLockScript;
};

class Transaction {
protected:
    std::vector<TransactionInput> mInputs;
    std::vector<TransactionOutput> mOutputs;
    uint32_t mLockTime;
};

class Block {
protected:
    uint32_t mTimeOffset;
    uint32_t mNonce;
    std::vector<Transaction> mTransactions;
};

} // compress