#include <bkbase/endian.h>
#include <bkbase/logging.h>
#include "block_file.h"

static std::string getFilename(int fileNumber) {
    char filename[32];
    snprintf(filename, sizeof(filename), "blk%05d.dat", fileNumber);
    return std::string(filename);
}

BlockFile::BlockFile(const char *blockDirectory)
    : mBlockDirectory(blockDirectory), mFileNumber(0), mFilePosition(0), mIsEnded(false)
{
}

bool BlockFile::_NextBlock(std::vector<uint8_t> &blockData) {
    while (!mIsEnded) {
        if (!hasBytes(1)) {
            std::string nextFile = (mBlockDirectory / getFilename(mFileNumber++)).string();
            BKINFO() << "reading " << nextFile;
            FILE *fp = fopen(nextFile.c_str(), "rb");
            if (fp == nullptr) {
                mIsEnded = true;
                break;
            }
            fseek(fp, 0, SEEK_END);
            long size = ftell(fp);
            if (size <= 0) {
                mIsEnded = true;
                fclose(fp);
                break;
            }
            mFileBuffer.resize((size_t)size);
            fseek(fp, 0, SEEK_SET);
            fread(&mFileBuffer[0], mFileBuffer.size(), 1, fp);
            fclose(fp);
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