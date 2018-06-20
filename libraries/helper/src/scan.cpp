#include <helper/scan.h>

namespace helper {

Scan::Scan(const char *path)
    : mLoader(path) {
}

void Scan::_ThreadProc(std::string name) {
    bkbase::LoggingThreadName(name);
    BKLOGSCOPE("entry");

    std::vector<uint8_t> blockBuffer;

    while (mLoader.NextBlock(blockBuffer)) {
        try {
            core::Block block;
            bkbase::StreamVector stream(bkbase::SER_DISK, 0, blockBuffer);
            stream >> block;
            OnBlock(block);
        } catch (bkbase::Exception &ex) {
            BKERROR() << boost::diagnostic_information(ex);
        } catch (std::exception &ex) {
            BKERROR() << ex.what();
        } catch (...) {
            BKERROR() << "Unknown Excpetion";
        }
    }

    BKINFO() << name << " Exited";
}

void Scan::Start() {
    boost::thread_group group;
    for (size_t i = 0; i < boost::thread::hardware_concurrency() + 1; i++) {
        std::stringstream ss;
        std::string threadName;
        ss << "scan";
        ss << i;
        ss >> threadName;

        group.create_thread(boost::bind(&Scan::_ThreadProc, this, threadName));
    }
    mLoader.LoadLoop();
    group.join_all();
}

ScanFast::ScanFast(const char *path, OutputPool *pool)
    : Scan(path), mPool(pool)
{}

void ScanFast::OnBlock(core::Block &blk) {
    auto &txs = blk.Transactions();
    for (auto txIter = txs.begin(); txIter != txs.end(); ++txIter) {
        auto tx = *txIter;
        bkbase::Hash256 txHash = tx->CalcHash();
        auto &outputs = tx->Outputs();
        for (size_t i = 0; i < outputs.size(); i++) {
            mPool->AddOutput(txHash, (uint16_t)i, outputs[i].LockScript(), outputs[i].Amount());
        }
        
        if (txIter != txs.begin()) {
            auto &inputs = tx->Inputs();
            for (auto i = inputs.begin(); i != inputs.end(); ++i) {
                mPool->AddInput(i->TxInID(), (uint16_t)i->TxInIndex(), i->UnlockScript());
            }
        }
    }
}

void ScanFast::Start() {
    Scan::Start();
    mPool->End();
}


ScanOrder::ScanOrder(const char *path, OutputPool *pool)
    : ScanFast(path, pool), mHeight(0), mPendingReported(0)
{}

ScanOrder::~ScanOrder() {
    BKINFO() << "Scan count:" << mHeight << "  id:" << mLastBlock.GetReverseHex();
}

void ScanOrder::OnBlock(core::Block &blk) {
    if (blk.HashPrevBlock() != mLastBlock) {
        mPendingBlocks.insert(std::make_pair(
            blk.HashPrevBlock(),
            std::move(blk)
        ));
        if ((mPendingBlocks.size() % 1000) == 0 && mPendingBlocks.size() > mPendingReported) {
            mPendingReported = mPendingBlocks.size();
            BKINFO() << "Pending blocks:" << mPendingReported;
        }
        return;
    }
    mLastBlock = blk.CalcHash();
    mHeight++;
    ScanFast::OnBlock(blk);
    auto finded = mPendingBlocks.find(mLastBlock);
    while (finded != mPendingBlocks.end()) {
        mLastBlock = finded->second.CalcHash();
        mHeight++;
        ScanFast::OnBlock(finded->second);
        mPendingBlocks.erase(finded);
        finded = mPendingBlocks.find(mLastBlock);
    }
}

void ScanOrder::Start() {
    boost::thread_group group;
    group.create_thread(boost::bind(&Scan::_ThreadProc, this, "scan"));
    mLoader.LoadLoop();
    group.join_all();
    mPool->End();
}

}