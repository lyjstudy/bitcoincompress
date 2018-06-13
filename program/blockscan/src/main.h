#pragma once

#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <block.h>
#include <block_loader.h>
#include <script/type.h>
#include <script/opcode.h>
#include <script/parser.h>

class OutputKey {
private:
    core::Uint256 mHashTxId;
    uint32_t mIndexID;
public:
    OutputKey(const core::Uint256 &txid, uint32_t index)
        : mHashTxId(txid), mIndexID(index)
    {}
    OutputKey(core::Uint256 &&txid, uint32_t index)
        : mHashTxId(txid), mIndexID(index)
    {}

    friend inline bool operator==(const OutputKey &self, const OutputKey &other) {
        return self.mIndexID == other.mIndexID &&
                self.mHashTxId == other.mHashTxId;
    }

    std::string GetHashString() const {
        return mHashTxId.GetHex();
    }
    uint32_t GetIndex() const {
        return mIndexID;
    }
    size_t GetHash() const {
        size_t r = mIndexID << 6;
        size_t txid = core::MemAs<size_t>(mHashTxId.begin());
        return r | (txid << 6);
    }
};
struct OutputKeyOperator {
    size_t operator()(const OutputKey &hash) const { return hash.GetHash(); }
};

class OutputMap {
public:
    using LogFuncType = std::function<void(const char *, uint32_t, script::Type, const std::vector<uint8_t> &, const std::vector<uint8_t> *)>;
protected:
    std::unordered_map<OutputKey, std::pair<script::Type, std::vector<uint8_t>>, OutputKeyOperator> mMap;
    // std::unordered_map<OutputKey, std::vector<uint8_t>, OutputKeyOperator> mPausedInput;
    LogFuncType mLogger;
    size_t mFlushByInput = 0;
    size_t mFlushByOutput = 0;
public:
    OutputMap(LogFuncType logger) : mLogger(logger) {}
    void AddOutput(const core::TransactionRef &tx, uint32_t index, script::Type type, const std::vector<uint8_t> &script) {
        auto key = OutputKey(tx->GetHash(), index);
        /*
        auto finded = mPausedInput.find(key);
        if (finded != mPausedInput.end()) {
            // outputtxid: key.GetHashString()
            // outputindex: index
            // outputtype: type
            // lockscript: script
            // unlockscript: finded->second
            mFlushByOutput++;
            mLogger(key.GetHashString().c_str(), index, type, script, &finded->second);
        } else {
            mMap.insert(std::make_pair(key, std::make_pair(type, script)));
        }
        */
       mMap.insert(std::make_pair(key, std::make_pair(type, script)));
    }
    void AddInput(const core::Uint256 &txid, uint32_t index, const std::vector<uint8_t> &script) {
        auto key = OutputKey(txid, index);
        auto finded = mMap.find(key);
        if (finded != mMap.end()) {
            // outputtxid: key.GetHashString()
            // outputindex: index
            // outputtype: type
            // lockscript: finded->second
            // unlockscript: script
            mFlushByInput++;
            mLogger(key.GetHashString().c_str(), index, finded->second.first, finded->second.second, &script);
        } else {
            // mPausedInput.insert(std::make_pair(key, script));
        }
    }
    void AddOpReturn(const core::TransactionRef &tx, uint32_t index, const std::vector<uint8_t> &script) {
        mLogger(tx->GetHash().GetHex().c_str(), index, script::Type::OpReturn, script, nullptr);
    }
    inline void Status() {
        // printf("Input: %zd %zd, Output: %zd %zd\n", mPausedInput.size(), mFlushByInput, mMap.size(), mFlushByOutput);
        printf("Output: %zd %zd\n", mMap.size(), mFlushByInput);
    }
};
