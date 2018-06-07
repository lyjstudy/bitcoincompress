#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <block.h>
#include <logging.h>
#include "compress.h"


class BlockPool {
protected:
    // Unknown Block
    std::unordered_map<core::Uint256, core::Block *, core::HashOperator<core::Uint256>> mUnknownBlock;
    // Block Hash => height
    std::unordered_map<core::Uint256, int, core::HashOperator<core::Uint256>> mMapHeight;
    // vector index = block height
    std::vector<std::set<core::Uint256>> mBlockTree;

    bool _addToHeight(const core::Uint256 &hash, int height);

    void _processBlock(core::Block *blk, int height);

    bool _writeCompress(core::Block *blk, int height);

protected:
    CompressBlockInfo mBlockInfo;
    int mLastBlockTime;
public:
    BlockPool();
    ~BlockPool();

    void AddBlock(core::Block *blk);

    void Dump() {
        mBlockInfo.Dump();
        LOGI("BlockPool Paused(%d), Processed(%d), Height(%d)", (int)mUnknownBlock.size(), (int)mMapHeight.size(), (int)mBlockTree.size() - 1);
    }
};
