#pragma once

#include <stdint.h>
#include <vector>
#include <mutex>
#include <deque>
#include <thread>
#include <boost/filesystem.hpp>
#include <bkbase/logging.h>

namespace helper {

constexpr uint8_t BLOCK_DISK_MAGIC[] = {0xF9, 0xBE, 0xB4, 0xD9};

class BlockLoader {
    static const size_t MAX_BUF_COUNT = 7;
protected:
    boost::filesystem::path mDirectory;
    int mFileNumber;
    bool mReachEnd;

    std::deque<std::vector<uint8_t>> mDequeBuffer;
    std::mutex mDequeMutex;
    inline bool IsEmpty() {
        std::lock_guard<std::mutex> guard(mDequeMutex);
        return mDequeBuffer.empty();
    }
    inline bool IsFull() {
        std::lock_guard<std::mutex> guard(mDequeMutex);
        return mDequeBuffer.size() >= MAX_BUF_COUNT;
    }
    inline void Push(std::vector<uint8_t> &&buffer) {
        std::lock_guard<std::mutex> guard(mDequeMutex);
        mDequeBuffer.push_back(buffer);
    }
    inline void Pop(std::vector<uint8_t> &buf) {
        std::lock_guard<std::mutex> guard(mDequeMutex);
        buf = std::move(mDequeBuffer.front());
        mDequeBuffer.pop_front();
    }
public:
    BlockLoader(const char *path);
    bool LoadNextFile(std::vector<uint8_t> &buffer);
    void LoadLoop();
    bool GetBuffer(std::vector<uint8_t> &buf);
};

class BlockFile {
protected:
    BlockLoader mLoader;

    size_t mFilePosition;
    std::vector<uint8_t> mFileBuffer;
    std::mutex mNextMutex;

    bool _NextBlock(std::vector<uint8_t> &blockData);

    inline bool hasBytes(size_t size) {
        return (mFilePosition + size) <= mFileBuffer.size();
    }
public:
    BlockFile(const char *blockDirectory);

    inline void LoadLoop() {
        mLoader.LoadLoop();
    }

    inline bool NextBlock(std::vector<uint8_t> &blockData) {
        std::lock_guard<std::mutex> guard(mNextMutex);
        try {
            return _NextBlock(blockData);
        } catch (...) {
            BKCRITICAL() << "BAD Exception Catch";
        }
        return false;
    }
};

}