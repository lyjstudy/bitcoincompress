
#include "block_loader.h"

namespace utils {

    #define LOG_FILE_ERROR(api) LOGE(api "(%d): %s", mFileNumber, strerror(errno))


    BlockLoader::BlockLoader(const char *dataDirectory)
        : mFileNumber(-1), mDataDirectory(dataDirectory), mFile(nullptr)
    {}

    BlockLoader::~BlockLoader() {
        _close();
    }

    void BlockLoader::_close() {
        if (mFile != nullptr) {
            fclose(mFile);
            mFile = nullptr;
        }
    }

    std::string BlockLoader::_getFilename(int fileNumber) {
        char filename[32];
        snprintf(filename, sizeof(filename), "blk%05d.dat", fileNumber);
        return mDataDirectory + std::string(filename);
    }

    void BlockLoader::Rewind() {
        mFileNumber = -1;
        _close();
    }

    bool BlockLoader::SetFilePos(int fileNumber, uint32_t position) {
        FILE *fp = fopen(_getFilename(fileNumber).c_str(), "rb");
        if (fp == nullptr) return false;
        if (fseek(fp, (long)position, SEEK_SET) != 0) {
            LOG_FILE_ERROR("fseek");
            fclose(fp);
            return false;
        }
        _close();
        mFileNumber = fileNumber;
        mFile = fp;
        return true;
    }

    bool BlockLoader::_skipMagic() {
        for (int i = 0; i < (int)sizeof(BLOCK_DISK_MAGIC); i++) {
            int val = fgetc(mFile);
            if (val == EOF) return false;

            if ((uint8_t)val != BLOCK_DISK_MAGIC[i]) {
                if (i != 0) {
                    if (fseek(mFile, -i, SEEK_CUR) != 0) {
                        LOG_FILE_ERROR("fseek");
                        return false;
                    }
                }
                i = -1;
            }
        }
        return true;
    }

    bool BlockLoader::NextBlock(std::function<void()> blockDetect) {
        while (true) {
            if (mFile == nullptr || feof(mFile)) {
                _close();
                mFileNumber++;
                mFile = fopen(_getFilename(mFileNumber).c_str(), "rb");
                if (mFile == nullptr) {
                    LOG_FILE_ERROR("fopen");
                    return false;
                }
                LOGI("reading block file: %d", mFileNumber);
            }

            if (!_skipMagic()) {
                _close();
                continue;
            }

            uint32_t size;
            if (fread(&size, 1, sizeof(size), mFile) != sizeof(size)) {
                LOG_FILE_ERROR("fread");
                _close();
                continue;
            }
            long nextPos = ftell(mFile) + size;

            if (fseek(mFile, -4, SEEK_CUR) != 0) {
                LOG_FILE_ERROR("fseek");
                _close();
                continue;
            }
            if (size < 80) {
                continue; // continue this file
            }
            
            blockDetect();

            if (fseek(mFile, nextPos, SEEK_SET) != 0) {
                LOG_FILE_ERROR("fseek");
            }
            break;
        }
        return true;
    }

    bool BlockLoader::_readBuffer(uint32_t size) {
        if (mFile == nullptr) return false;

        if (size == 0) {
            if (fread(&size, 1, sizeof(size), mFile) != sizeof(size)) {
                LOG_FILE_ERROR("fread");
                return false;
            }
        } else {
            if (fseek(mFile, 4, SEEK_CUR) != 0) {
                LOG_FILE_ERROR("fseek");
                return false;
            }
        }

        mBuffer.resize(size);

        if (fread(&mBuffer[0], 1, size, mFile) != size) {
            LOG_FILE_ERROR("fread");
            return false;
        }
        return true;
    }

    bool BlockLoader::ReadBlock(core::Block &blk) {
        if (!_readBuffer()) return false;

        core::VectorStream stream(core::SER_DISK, CLIENT_VERSION, mBuffer);
        
        if (!core::Unserialize(stream, blk)) {
            LOGE("core::Unserialize block failure");
            return false;
        }

        return true;
    }

    bool BlockLoader::ReadHeader(core::BlockHeader &hdr) {
        if (!_readBuffer(80)) return false;

        core::VectorStream stream(core::SER_DISK, CLIENT_VERSION, mBuffer);
        
        if (!core::Unserialize(stream, hdr)) {
            LOGE("core::Unserialize blockheader failure");
            return false;
        }

        return true;
    }

}