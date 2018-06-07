#pragma once

#include <stdint.h>
#include <block.h>
#include <logging.h>

class CompressBlock {
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline bool SerializationOp(Stream &s, Operation serAction) {
        READWRITEVARINT(mOffset);
        READWRITE(mNonce);
        return true;
    }
protected:
    uint32_t mOffset; // mOffset = (prev_block.time + 600) - block.time)
    uint32_t mNonce;

    std::vector<uint64_t> mCoinbaseOutput;
public:
    CompressBlock();

    void SetBlock(core::Block *blk, int prevTime);
};

class CompressBlockInfo {
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline bool SerializationOp(Stream &s, Operation serAction) {
        READWRITE(mBlockVersionHistory);
        READWRITE(mBlockBitsHistory);
        READWRITE(mTxVersionHistory);
        return true;
    }
protected:
    std::vector<std::pair<uint32_t, int32_t>> mBlockVersionHistory;
    std::vector<std::pair<uint32_t, uint32_t>> mBlockBitsHistory;
    std::vector<std::pair<uint32_t, int32_t>> mCoinbaseVersionHistory;
    std::vector<std::pair<uint32_t, int32_t>> mTxVersionHistory;

    template<typename T1, typename T2, typename T3>
    void addHistory(T3 &history, T1 first, T2 second) {
        if (history.empty() || history.back().second != second) {
            // history.push_back(std::make_pair(first, second));
        }
    }
public:
    CompressBlockInfo();
    
    void AddBlock(uint32_t height, core::Block *blk);
    void AddTransation(uint32_t index, core::Transaction *tx);

    void Dump() {
        LOGI("BlockInfo BlkVersion(%d), BlkBits(%d), TxVersion(%d), CbVersion(%d)", (int)mBlockVersionHistory.size(),
            (int)mBlockBitsHistory.size(), (int)mTxVersionHistory.size(), (int)mCoinbaseVersionHistory.size());
    }
};
