#pragma once

#include "serialize.h"
#include "transaction.h"

namespace core {

    class BlockHeader {
        ADD_SERIALIZE_METHODS;
        template <typename Stream, typename Operation>
        inline bool SerializationOp(Stream &s, Operation serAction) {
            READWRITE(mVersion);
            READWRITE(mHashPrevBlock);
            READWRITE(mMerkleRoot);
            READWRITE(mTime);
            READWRITE(mBits);
            READWRITE(mNonce);
            return true;
        }
    protected:
        int32_t mVersion;
        Uint256 mHashPrevBlock;
        Uint256 mMerkleRoot;
        uint32_t mTime;
        uint32_t mBits;
        uint32_t mNonce;
    public:
        BlockHeader(const BlockHeader &other)
            : mVersion(other.mVersion),
            mHashPrevBlock(other.mHashPrevBlock),
            mMerkleRoot(other.mMerkleRoot),
            mTime(other.mTime),
            mBits(other.mBits),
            mNonce(other.mNonce)
        {}
        BlockHeader() { SetNull(); }

        inline int32_t Version() {
            return mVersion;
        }
        inline uint32_t Bits() {
            return mBits;
        }
        inline uint32_t Time() {
            return mTime;
        }
        inline uint32_t Nonce() {
            return mNonce;
        }

        inline const Uint256 &HashPrevBlock() {
            return mHashPrevBlock;
        }

        inline void SetNull() {
            mVersion = 0;
            mHashPrevBlock.SetZero();
            mMerkleRoot.SetZero();
            mTime = 0;
            mBits = 0;
            mNonce = 0;
        }

        inline bool IsNull() const { return (mBits == 0); }

        inline Uint256 GetHash() const {
            DataStream stream(0, 0);
            this->Serialize(stream);
            return HashDoubleSha256(stream.Buffer());
        }

        inline int64_t GetBlockTime() const { return (int64_t)mTime; }
    };

    class Block : public BlockHeader {
        ADD_SERIALIZE_METHODS;
        template <typename Stream, typename Operation>
        inline bool SerializationOp(Stream &s, Operation serAction) {
            READWRITE(*(BlockHeader *)this);
            READWRITE(mTransactions);
            return true;
        }
    protected:
        std::vector<TransactionRef> mTransactions;
    public:
        Block(const Block &) = delete;
        Block(Block &&) = delete;
        Block() { SetNull(); }
        Block(const BlockHeader &header) : BlockHeader(header) {
            mTransactions.clear();
        }
        void SetNull() {
            BlockHeader::SetNull();
            mTransactions.clear();
        }
        size_t GetTxCount() {
            return mTransactions.size();
        }
        TransactionRef GetTx(size_t idx) {
            if (idx >= mTransactions.size()) {
                return TransactionRef();
            }
            return mTransactions[idx];
        }
    };

}