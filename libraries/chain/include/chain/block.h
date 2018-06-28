#pragma once

#include <bkbase/serialize.h>
#include <bkbase/hash.h>
#include <crypto/hasher.h>
#include "transaction.h"

namespace chain {

class BlockHeader {
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation serAction) {
        READWRITE(mVersion);
        READWRITE(mHashPrevBlock);
        READWRITE(mMerkleRoot);
        READWRITE(mTime);
        READWRITE(mBits);
        READWRITE(mNonce);
    }
protected:
    int32_t mVersion;
    bkbase::Hash256 mHashPrevBlock;
    bkbase::Hash256 mMerkleRoot;
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
    BlockHeader(BlockHeader &&other)
        : mVersion(other.mVersion),
        mHashPrevBlock(other.mHashPrevBlock),
        mMerkleRoot(other.mMerkleRoot),
        mTime(other.mTime),
        mBits(other.mBits),
        mNonce(other.mNonce)
    {}
    BlockHeader() { SetNull(); }

    inline int32_t Version() const {
        return mVersion;
    }
    inline uint32_t Bits() const {
        return mBits;
    }
    inline uint32_t Time() const {
        return mTime;
    }
    inline uint32_t Nonce() const {
        return mNonce;
    }
    inline const bkbase::Hash256 &MerkleRoot() const {
        return mMerkleRoot;
    }
    inline const bkbase::Hash256 &HashPrevBlock() const {
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

    inline bkbase::Hash256 CalcHash() const {
        crypto::StreamHash256 stream(bkbase::SER_GETHASH, 0);
        stream << *this;
        return stream.GetHash();
    }
};

class Block : public BlockHeader {
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation serAction) {
        READWRITE(*(BlockHeader *)this);
        READWRITE(mTransactions);
    }
protected:
    std::vector<TransactionRef> mTransactions;
public:
    Block() {}
    Block(const Block &other)
        : BlockHeader(other), mTransactions(other.mTransactions)
    {}
    Block(Block &&other)
        : BlockHeader(other), mTransactions(other.mTransactions)
    {}
    void SetNull() {
        BlockHeader::SetNull();
        mTransactions.clear();
    }
    inline const std::vector<TransactionRef> &Transactions() const {
        return mTransactions;
    }
};

} // chain