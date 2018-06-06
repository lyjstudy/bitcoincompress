#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include "serialize.h"
#include "hash.h"

namespace core {

    class OutPoint {
        ADD_SERIALIZE_METHODS;
        template <typename Stream, typename Operation>
        inline bool SerializationOp(Stream &s, Operation serAction) {
            READWRITE(mTxId);
            READWRITE(mIndex);
            return true;
        }
    protected:
        Uint256 mTxId;
        uint32_t mIndex;
    public:
        OutPoint() : mTxId(), mIndex(-1) {}
        OutPoint(Uint256 txid, uint32_t idx) : mTxId(txid), mIndex(idx) {}
    };

    class TransactionIn {
        ADD_SERIALIZE_METHODS;
        template <typename Stream, typename Operation>
        inline bool SerializationOp(Stream &s, Operation serAction) {
            READWRITE(mPrevOut);
            READWRITE(mUnlockScript);
            READWRITE(mSequence);
            return true;
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
         * lock-time.scriptSigIn
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
        OutPoint mPrevOut;
        std::vector<uint8_t> mUnlockScript;
        uint32_t mSequence;
    public:
        TransactionIn() : mSequence(SEQUENCE_FINAL) {}

        explicit TransactionIn(const OutPoint &prevout, const std::vector<uint8_t> &unlockScript,
                            uint32_t sequence = SEQUENCE_FINAL)
            : mPrevOut(prevout), mUnlockScript(unlockScript), mSequence(sequence) {}

    };

    class TransactionOut {
        ADD_SERIALIZE_METHODS;
        template <typename Stream, typename Operation>
        inline bool SerializationOp(Stream &s, Operation serAction) {
            READWRITE(mAmount);
            READWRITE(mLockScript);
            return true;
        }
    protected:
        int64_t mAmount;
        std::vector<uint8_t> mLockScript;
    public:
        TransactionOut() : mAmount(0) {}
        TransactionOut(int64_t amount, const std::vector<uint8_t> &lockScript)
            : mAmount(amount), mLockScript(lockScript) {}
        const std::vector<uint8_t> &GetScript() {
            return mLockScript;
        }
    };

    class Transaction {
        ADD_SERIALIZE_METHODS;
        template <typename Stream, typename Operation>
        inline bool SerializationOp(Stream &s, Operation serAction) {
            if (serAction.ForRead()) {
                mInputs.clear();
                mOutputs.clear();
            }
            READWRITE(mVersion);
            READWRITE(mInputs);
            READWRITE(mOutputs);
            READWRITE(mLockTime);
            return true;
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
        std::vector<TransactionIn> mInputs;
        std::vector<TransactionOut> mOutputs;
        uint32_t mLockTime;
    public:
        Transaction();
        inline int32_t Version() { return mVersion; }
        inline uint32_t LockTime() { return mLockTime; }
        inline size_t GetInputSize() { return mInputs.size(); }
        inline size_t GetOutputSize() { return mOutputs.size(); }
        inline const TransactionOut &GetOutput(size_t idx) {
            return mOutputs[idx];
        }
    };

    typedef std::shared_ptr<Transaction> TransactionRef;
    static inline TransactionRef MakeTransactionRef() {
        return std::make_shared<Transaction>();
    }
    template <typename Tx>
    static inline TransactionRef MakeTransactionRef(Transaction &&txIn) {
        return std::make_shared<Transaction>(std::forward<Transaction>(txIn));
    }
}