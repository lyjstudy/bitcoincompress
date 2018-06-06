#include "compress.h"


CompressBlock::CompressBlock() {
}

void CompressBlock::SetBlock(core::Block *blk, int prevTime) {
    int timeOff = prevTime + 600 - (int)blk->Time();
    if (timeOff < 0) {
        timeOff = ((-timeOff) << 1) | 1;
    } else {
        timeOff <<= 1;
    }
    mOffset = (uint32_t)timeOff;
    mNonce = blk->Nonce();

    // Save coinbase
}

CompressBlockInfo::CompressBlockInfo() {
}
    
void CompressBlockInfo::AddBlock(uint32_t height, core::Block *blk) {
    addHistory(mBlockVersionHistory, height, blk->Version());
    addHistory(mBlockBitsHistory, height, blk->Bits());
    addHistory(mCoinbaseVersionHistory, height, blk->GetTx(0)->Version());
}

void CompressBlockInfo::AddTransation(uint32_t index, core::Transaction *tx) {
    addHistory(mTxVersionHistory, index, tx->Version());
}
