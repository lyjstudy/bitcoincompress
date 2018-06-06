#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <block.h>
#include <config.h>
#include <logging.h>

namespace utils {


    class BlockLoader {
    protected:
        int mFileNumber;
        std::string mDataDirectory;
        FILE *mFile;
        std::vector<uint8_t> mBuffer;

        void _close();
        std::string _getFilename(int fileNumber);
        bool _skipMagic();
        bool _readBuffer(uint32_t size = 0); 
    public:
        BlockLoader(const char *dataDirectory);
        ~BlockLoader();

        inline int FileNumber() { return mFileNumber; }

        void Rewind();
        bool SetFilePos(int fileNumber, uint32_t position);

        bool NextBlock(std::function<void()> blockDetect);

        bool ReadBlock(core::Block &blk);
        bool ReadHeader(core::BlockHeader &hdr);
    };

}