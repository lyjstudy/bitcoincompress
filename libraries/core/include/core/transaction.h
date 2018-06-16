#pragma once

#include <memory>
#include <bkbase/serialize.h>
#include <bkbase/hash.h>
#include <crypto/hasher.h>

namespace core {

class TransactionInput {
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation serAction) {
        READWRITE(mTxInID);
        READWRITE(mTxInIndex);
        READWRITE(mUnlockScript);
        READWRITE(mSequence);
    }
public:
    /**
     * Setting nSequence to this value for every input in a transaction disables
     * nLockTime.
     */
    static const uint32_t SEQUENCE_FINAL = 0xffffffff;

    /* Below flags apply in the context of BIP 68*/
    /**
     * If this flag set, CTxIn::nSequence is NOT interpreted as a relative
     * lock-time.
     */
    static const uint32_t SEQUENCE_LOCKTIME_DISABLE_FLAG = (1 << 31);

    /**
     * If CTxIn::nSequence encodes a relative lock-time and this flag is set,
     * the relative lock-time has units of 512 seconds, otherwise it specifies
     * blocks with a granularity of 1.
     */
    static const uint32_t SEQUENCE_LOCKTIME_TYPE_FLAG = (1 << 22);

    /**
     * If CTxIn::nSequence encodes a relative lock-time, this mask is applied to
     * extract that lock-time from the sequence field.
     */
    static const uint32_t SEQUENCE_LOCKTIME_MASK = 0x0000ffff;

    /**
     * In order to use the same number of bits to encode roughly the same
     * wall-clock duration, and because blocks are naturally limited to occur
     * every 600s on average, the minimum granularity for time-based relative
     * lock-time is fixed at 512 seconds. Converting from CTxIn::nSequence to
     * seconds is performed by multiplying by 512 = 2^9, or equivalently
     * shifting up by 9 bits.
     */
    static const int SEQUENCE_LOCKTIME_GRANULARITY = 9;
protected:
    bkbase::Hash256 mTxInID;
    uint32_t mTxInIndex;
    std::vector<uint8_t> mUnlockScript;
    uint32_t mSequence;
public:
    TransactionInput()
        : mTxInIndex(-1), mSequence(SEQUENCE_FINAL)
    {}
    TransactionInput(const TransactionInput &other)
        : mTxInID(other.mTxInID), mTxInIndex(other.mTxInIndex)
        , mUnlockScript(other.mUnlockScript), mSequence(other.mSequence)
    {}
    TransactionInput(TransactionInput &&other)
        : mTxInID(other.mTxInID), mTxInIndex(other.mTxInIndex)
        , mUnlockScript(other.mUnlockScript), mSequence(other.mSequence)
    {}
    inline const bkbase::Hash256 &TxInID() const {
        return mTxInID;
    }
    inline uint32_t TxInIndex() const {
        return mTxInIndex;
    }
    inline const std::vector<uint8_t> &UnlockScript() const {
        return mUnlockScript;
    }
    inline uint32_t Sequence() const {
        return mSequence;
    }
};

class TransactionOutput {
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation serAction) {
        READWRITE(mAmount);
        READWRITE(mLockScript);
    }
protected:
    int64_t mAmount;
    std::vector<uint8_t> mLockScript;
public:
    TransactionOutput() : mAmount(-1) {}
    TransactionOutput(const TransactionOutput &other)
        : mAmount(other.mAmount), mLockScript(other.mLockScript)
    {}
    TransactionOutput(TransactionOutput &&other)
        : mAmount(other.mAmount), mLockScript(other.mLockScript)
    {}
    inline int64_t Amount() const {
        return mAmount;
    }
    inline const std::vector<uint8_t> &LockScript() const {
        return mLockScript;
    }
};

class Transaction {
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation serAction) {
        READWRITE(mVersion);
        READWRITE(mInputs);
        READWRITE(mOutputs);
        READWRITE(mLockTime);
    }
public:
    // Default transaction version.
    static const int32_t CURRENT_VERSION = 2;

    // Changing the default transaction version requires a two step process:
    // first adapting relay policy by bumping MAX_STANDARD_VERSION, and then
    // later date bumping the default CURRENT_VERSION at which point both
    // CURRENT_VERSION and MAX_STANDARD_VERSION will be equal.
    static const int32_t MAX_STANDARD_VERSION = 2;
protected:
    int32_t mVersion;
    std::vector<TransactionInput> mInputs;
    std::vector<TransactionOutput> mOutputs;
    uint32_t mLockTime;
public:
    template<typename Stream>
    Transaction(bkbase::deserialize_type, Stream &s) {
        Unserialize(s);
    }
    Transaction() : mVersion(CURRENT_VERSION), mLockTime(0) {}
    Transaction(const Transaction &other)
        : mVersion(other.mVersion), mInputs(other.mInputs)
        , mOutputs(other.mOutputs), mLockTime(other.mLockTime)
    {}
    Transaction(Transaction &&other)
        : mVersion(other.mVersion), mInputs(other.mInputs)
        , mOutputs(other.mOutputs), mLockTime(other.mLockTime)
    {}
    inline int32_t Version() const {
        return mVersion;
    }
    inline uint32_t LockTime() const {
        return mLockTime;
    }
    inline const std::vector<TransactionInput> &Inputs() const {
        return mInputs;
    }
    inline const std::vector<TransactionOutput> &Outputs() const {
        return mOutputs;
    }
    inline bkbase::Hash256 CalcHash() const {
        crypto::StreamHash256 stream(bkbase::SER_GETHASH, 0);
        stream << *this;
        return stream.GetHash();
    }
};

typedef std::shared_ptr<const Transaction> TransactionRef;
static inline TransactionRef MakeTransactionRef() {
    return std::make_shared<const Transaction>();
}
template <typename Tx>
static inline TransactionRef MakeTransactionRef(Tx &&txIn) {
    return std::make_shared<const Transaction>(std::forward<Tx>(txIn));
}

}