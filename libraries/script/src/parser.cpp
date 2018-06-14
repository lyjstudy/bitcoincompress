#include <script/parser.h>

namespace script {

    Parser::Parser(const std::vector<uint8_t> &codes)
        : mCodes(codes), mPc(0)
    {}

    bool Parser::Fetch(uint8_t &opcodeRet, std::vector<uint8_t> *data) {
        opcodeRet = OP_INVALIDOPCODE;
        if (data != nullptr) data->clear();
        if (!_hasBytes(1)) return false;

        opcodeRet = mCodes[mPc++];
        if (opcodeRet <= OP_PUSHDATA4) {
            uint32_t size = 0;
            if (opcodeRet < OP_PUSHDATA1) {
                size = opcodeRet;
            } else if (opcodeRet == OP_PUSHDATA1) {
                if (!_hasBytes(1)) return false;
                size = mCodes[mPc++];
            } else if (opcodeRet == OP_PUSHDATA2) {
                if (!_hasBytes(2)) return false;
                size = bkbase::ReadLE<uint16_t>(&mCodes[mPc]);
                mPc += 2;
            } else if (opcodeRet == OP_PUSHDATA4) {
                if (!_hasBytes(4)) return false;
                size = bkbase::ReadLE<uint32_t>(&mCodes[mPc]);
                mPc += 4;
            }
            if (size != 0) {
                if (!_hasBytes(size)) return false;
                if (data) data->assign(&mCodes[mPc], &mCodes[mPc + size]);
                mPc += size;
            }
        }
        return true;
    }

    size_t Parser::GetTemplate(const std::vector<uint8_t> &codes, std::vector<uint8_t> &temp, 
                        std::vector<std::vector<uint8_t>> *pushData) {
        temp.clear();
        if (pushData != nullptr) pushData->clear();

        Parser parser(codes);
        std::vector<uint8_t> data;
        uint8_t opcode;

        if (pushData != nullptr) {
            while (parser.Fetch(opcode, &data)) {
                temp.push_back(opcode);
                if (!data.empty()) pushData->push_back(std::move(data));
            }
        } else {
            while (parser.Fetch(opcode)) {
                temp.push_back(opcode);
            }
        }

        if (opcode != OP_INVALIDOPCODE) {
            return parser.mPc;
        }
        return 0;
    }
}
