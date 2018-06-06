#include <block_pool.h>
#include <hex.h>
#include <script/type.h>

BlockPool::BlockPool() : mLastBlockTime(0) {

}

BlockPool::~BlockPool() {
    for (auto &blk : mUnknownBlock) {
        // printf("error block: %s\n", blk.first.GetHex().c_str());
        delete blk.second;
    }
    LOGI("Unhandled count: %d", (int)mUnknownBlock.size());

    mBlockInfo.Dump();
}

bool BlockPool::_addToHeight(const core::Uint256 &hash, int height) {
    if (mBlockTree.size() == (size_t)height) {
        mMapHeight.insert(std::make_pair(hash, height));
        mBlockTree.push_back(std::set<core::Uint256>{hash});
        return true;
    } else if (mBlockTree.size() > (size_t)height) {
        // mBlockTree[height].insert(hash);
        LOGE("IGNORE FORK BLOCK: %s(%d)", hash.GetHex().c_str(), height);
    } else {
        LOGE("BAD INSERT NO PREV BLOCK: %s(%d)\n", hash.GetHex().c_str(), height);
    }
    return false;
}

void BlockPool::AddBlock(core::Block *blk) {
    if (blk->HashPrevBlock().IsZero()) {
        _processBlock(blk, 0);
        delete blk;
        return;
    } else {
        auto finded = mMapHeight.find(blk->HashPrevBlock());
        if (finded != mMapHeight.end()) {
            _processBlock(blk, finded->second + 1);
            delete blk;
            return;
        }
    }
    mUnknownBlock.insert(std::make_pair(
        blk->GetHash(),
        blk
    ));
}


bool BlockPool::_writeCompress(core::Block *blk, int height) {

    mBlockInfo.AddBlock((uint32_t)height, blk);

    CompressBlock compress;
    compress.SetBlock(blk, mLastBlockTime);
    mLastBlockTime = blk->Time();


    auto hashStr = blk->GetHash().GetHex();
    for (size_t i = 0; i < blk->GetTxCount(); i++) {
        auto tx = blk->GetTx(i);
        for (size_t j = 0; j < tx->GetOutputSize(); j++) {
            auto out = tx->GetOutput(j);
            switch (script::GetOutputType(out.GetScript())) {
                case script::Type::OpReturn:
                    printf("OpReturn: %s(%d, %d)\n%s\n", hashStr.c_str(), (int)i, (int)j, core::HexFromBin(out.GetScript()).c_str());
                    break;
                case script::Type::NonStandard:
                    printf("NonStandard: %s(%d, %d)\n%s\n", hashStr.c_str(), (int)i, (int)j, core::HexFromBin(out.GetScript()).c_str());
                    break;
                default:
                    break;
            }
        }
    }

    return true;
}

void BlockPool::_processBlock(core::Block *blk, int height) {
    
    auto hash = blk->GetHash();

    // check conflict
    auto conflict = mMapHeight.find(hash);
    if (conflict != mMapHeight.end()) {
        LOGE("CONFLICT BLOCK: %s(%d)", hash.GetHex().c_str(), conflict->second);
        return;
    }
    // check coinbase
    auto coinbase = blk->GetTx(0);
    if (coinbase == nullptr) {
        LOGE("BAD BLOCK NO coinbase: %s", blk->GetHash().GetHex().c_str());
        return;
    }
    // check coinbase parameters
    if (coinbase->LockTime() != 0) {
        LOGE("UNSUPPORT COINBASE locktime(%d)", coinbase->LockTime());
        return;
    }

    if (!_addToHeight(hash, height)) return;

    if (!_writeCompress(blk, height)) return;

    // write paused children
    std::vector<core::Block *> childs;
    for (auto i = mUnknownBlock.begin(); i != mUnknownBlock.end(); ) {
        if (i->second->HashPrevBlock() == hash) {
            childs.push_back(i->second);
            i = mUnknownBlock.erase(i);
        } else {
            ++i;
        }
    }
    for (auto child : childs) {
        _processBlock(child, height + 1);
        delete child;
    }
}