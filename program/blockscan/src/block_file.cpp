#include <bkbase/endian.h>
#include <bkbase/logging.h>
#include "block_file.h"

static std::string getFilename(int fileNumber) {
    char filename[32];
    snprintf(filename, sizeof(filename), "blk%05d.dat", fileNumber);
    return std::string(filename);
}


BlockLoader::BlockLoader(const char *path)
    : mDirectory(path), mFileNumber(0), mReachEnd(false) {
}

bool BlockLoader::LoadNextFile(std::vector<uint8_t> &buffer) {
    std::string nextFile = (mDirectory / getFilename(mFileNumber++)).string();
    FILE *fp = fopen(nextFile.c_str(), "rb");
    if (fp == nullptr) {
        return false;
    }
    BKINFO() << "reading " << nextFile;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size <= 0) {
        fclose(fp);
        return false;
    }
    buffer.resize((size_t)size);
    fseek(fp, 0, SEEK_SET);
    fread(&buffer[0], buffer.size(), 1, fp);
    fclose(fp);
    return true;
}

void BlockLoader::LoadLoop() {
    while (true) {
        std::vector<uint8_t> buffer;
        if (!LoadNextFile(buffer)) break;

        while (IsFull()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        Push(std::move(buffer));
    }
    mReachEnd = true;
}

bool BlockLoader::GetBuffer(std::vector<uint8_t> &buf) {
    while (IsEmpty()) {
        if (mReachEnd) return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    Pop(buf);
    return true;
}

BlockFile::BlockFile(const char *blockDirectory)
    : mLoader(blockDirectory), mFilePosition(0)
{
}

bool BlockFile::_NextBlock(std::vector<uint8_t> &blockData) {
    while (true) {
        if (!hasBytes(1)) {
            if (!mLoader.GetBuffer(mFileBuffer)) break;
            mFilePosition = 0;
        }

        // skip magic
        while (true) {
            static_assert(sizeof(BLOCK_DISK_MAGIC) == 4, "BAD BLOCK_DISK_MAGIC");
            if (!hasBytes(4)) {
                mFilePosition = mFileBuffer.size();
                break;
            }
            if (mFileBuffer[mFilePosition++] != BLOCK_DISK_MAGIC[0]) continue;
            if (mFileBuffer[mFilePosition] != BLOCK_DISK_MAGIC[1]) continue;
            if (mFileBuffer[mFilePosition + 1] != BLOCK_DISK_MAGIC[2]) continue;
            if (mFileBuffer[mFilePosition + 2] != BLOCK_DISK_MAGIC[3]) continue;
            mFilePosition += 3;
            break;
        }

        // block size
        if (!hasBytes(sizeof(uint32_t))) {
            mFilePosition = mFileBuffer.size();
            continue;
        }
        uint32_t size = bkbase::ReadLE<uint32_t>(&mFileBuffer[mFilePosition]);
        mFilePosition += 4;
        if (size < 80) {
            continue;
        }
        if (!hasBytes(size)) {
            mFilePosition = mFileBuffer.size();
            continue;
        }

        blockData.resize(size);
        memcpy(&blockData[0], &mFileBuffer[mFilePosition], size);
        mFilePosition += size;

        return true;
    }
    return false;
}