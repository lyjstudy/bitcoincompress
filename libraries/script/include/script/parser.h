#pragma once

#include "opcode.h"
#include <serialize.h>

namespace script {

    class Parser {
    protected:
        std::vector<uint8_t> mCodes;
        size_t mPc;

        inline bool _hasBytes(size_t count) {
            if (count == 0) return mPc < mCodes.size();
            return mPc + count - 1 < mCodes.size();
        }
    public:
        explicit Parser(const std::vector<uint8_t> &codes);

        bool Fetch(uint8_t &opcodeRet, std::vector<uint8_t> *data = nullptr);

        static void GetScriptTemplate(const std::vector<uint8_t> &codes, std::vector<uint8_t> &temp, 
                        std::vector<std::vector<uint8_t>> *pushData = nullptr);
    };
}