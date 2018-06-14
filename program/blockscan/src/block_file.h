#pragma once

#include <stdint.h>
#include <vector>
#include <mutex>
#include <boost/filesystem.hpp>
#include <bkbase/logging.h>

constexpr uint8_t BLOCK_DISK_MAGIC[] = {0xF9, 0xBE, 0xB4, 0xD9};

class BlockFile {
protected:
    boost::filesystem::path mBlockDirectory;
    int mFileNumber;
    size_t mFilePosition;
    std::vector<uint8_t> mFileBuffer;
    bool mIsEnded;
    std::mutex mNextMutex;

    bool _NextBlock(std::vector<uint8_t> &blockData);

    inline bool hasBytes(size_t size) {
        return (mFilePosition + size) <= mFileBuffer.size();
    }
public:
    BlockFile(const char *blockDirectory);

    inline bool NextBlock(std::vector<uint8_t> &blockData) {
        std::lock_guard<std::mutex> guard(mNextMutex);
        try {
            return _NextBlock(blockData);
        } catch (...) {
            BKCRITICAL() << "BAD Exception Catch";
            mIsEnded = true;
        }
        return mIsEnded;
    }
};