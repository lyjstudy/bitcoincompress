#pragma once

#include <string>
#include <chain/block.h>
#include <helper/block_file.h>
#include <helper/output_pool.h>

namespace helper {

class Scan {
protected:
    BlockFile mLoader;

public:
    void _ThreadProc(std::string name);

    explicit Scan(const char *path);

    virtual void OnBlock(chain::Block &blk) = 0;

    virtual void Start();
};

class ScanFast : public Scan {
protected:
    OutputPool *mPool;
public:
    ScanFast(const char *path, OutputPool *pool);
    virtual void OnBlock(chain::Block &blk);
    virtual void Start();
};

class ScanOrder : public ScanFast {
protected:
    bkbase::Hash256 mLastBlock;
    std::unordered_map<bkbase::Hash256, chain::Block, bkbase::HashOperator> mPendingBlocks;
    std::mutex mMutex;
    size_t mHeight;
    size_t mPendingReported;
public:
    ScanOrder(const char *path, OutputPool *pool);
    ~ScanOrder();
    virtual void OnBlock(chain::Block &blk);
    virtual void Start();
};

}