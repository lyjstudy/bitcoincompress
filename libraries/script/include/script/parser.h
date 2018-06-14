#pragma once

#include "opcode.h"
#include <bkbase/endian.h>

namespace script {

    class Parser {
    protected:
        std::vector<uint8_t> mCodes;
        size_t mPc;

        inline bool _hasBytes(size_t count) {
            return mPc + count <= mCodes.size();
        }
    public:
        explicit Parser(const std::vector<uint8_t> &codes);

        bool Fetch(uint8_t &opcodeRet, std::vector<uint8_t> *data = nullptr);

        // 0 ok
        // > 0 error position in codes
        static size_t GetTemplate(const std::vector<uint8_t> &codes, std::vector<uint8_t> &temp, 
                        std::vector<std::vector<uint8_t>> *pushData = nullptr);
    };
}