#pragma once

#include <bkbase/hash.h>
#include <bkbase/logging.h>
#include <unordered_map>
#include <mutex>

namespace helper {

/*
    Output: txid index lockscript
    Input: txid index unlockscript
*/
class OutputKey : public bkbase::HashBase<256 + 16> {
public:
    using bkbase::HashBase<256 + 16>::HashBase;
    OutputKey(const bkbase::Hash256 &txid, uint16_t index) {
        mDataPtr = allocator.allocate(Size());
        bkbase::WriteLE<uint16_t>(mDataPtr, index);
        memcpy(mDataPtr + sizeof(uint16_t), txid.begin(), txid.size());
    }
    inline uint16_t TxIndex() const {
        if (mDataPtr == nullptr) return 0;
        return bkbase::ReadLE<uint16_t>(mDataPtr);
    }
    inline bkbase::Hash256 TxID() const {
        if (mDataPtr == nullptr) return bkbase::Hash256();
        return bkbase::Hash256(mDataPtr + sizeof(uint16_t), Size() - sizeof(uint16_t));
    }
};

class OutputPool {
protected:
    class Pool {
    protected:
        std::unordered_map<OutputKey, std::vector<uint8_t>, bkbase::HashOperator> mPending;
        std::mutex mMutex;
        std::string mName;
        size_t mCurrent;
    public:
        Pool(const std::string &name) : mName(name), mCurrent(0) {

        }
        bool ErasePending(const OutputKey &key, std::vector<uint8_t> &script) {
            std::lock_guard<std::mutex> gurad(mMutex);
            auto finded = mPending.find(key);
            if (finded == mPending.end()) {
                return false;
            }
            script = std::move(finded->second);
            mPending.erase(finded);
            return true;
        }
        void AddPending(const OutputKey &key, const std::vector<uint8_t> &script) {
            size_t dumpPending = 0;
            {
                std::lock_guard<std::mutex> gurad(mMutex);
                mPending.insert(std::make_pair(key, script));
                if (mPending.size() > mCurrent && (mPending.size() % 50000) == 0) {
                    mCurrent = mPending.size();
                    dumpPending = mCurrent;
                }
            }
            if (dumpPending > 0) {
                BKINFO() << mName << ": " << dumpPending;
            }
        }
        std::unordered_map<OutputKey, std::vector<uint8_t>, bkbase::HashOperator> &Data() {
            return mPending;
        }
    };
protected:
    Pool mOutputs;
    Pool mInputs;
public:
    OutputPool() : mOutputs("Outputs"), mInputs("Inputs") {}
    void AddOutput(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &script, int64_t amount) {
        if (!IsUnspendable(script, amount))
        {
            OutputKey key(txid, index);
            std::vector<uint8_t> unlockScript;
            if (!mInputs.ErasePending(key, unlockScript)) {
                mOutputs.AddPending(key, script);
            } else {
                OnTransaction(txid, index, script, unlockScript);
            }
        } else {
            OnOutputUnspendable(txid, index, script);
        }
    }
    void AddInput(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &script) {
        OutputKey key(txid, index);
        std::vector<uint8_t> lockScript;
        if (!mOutputs.ErasePending(key, lockScript)) {
            mInputs.AddPending(key, script);
        } else {
            OnTransaction(txid, index, lockScript, script);
        }
    }
    void End() {
        for (auto i = mInputs.Data().begin(); i != mInputs.Data().end(); ++i) {
            OnInput(i->first.TxID(), i->first.TxIndex(), i->second);
        }
        mInputs.Data().clear();
        for (auto i = mOutputs.Data().begin(); i != mOutputs.Data().end(); ++i) {
            OnOutput(i->first.TxID(), i->first.TxIndex(), i->second);
        }
        mOutputs.Data().clear();
    }
    // Thread Any
    virtual bool IsUnspendable(const std::vector<uint8_t> &script, int64_t amount) = 0;
    virtual void OnOutputUnspendable(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock) = 0;
    virtual void OnTransaction(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock, const std::vector<uint8_t> &unlock) = 0;
    // Thread Main
    virtual void OnInput(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &unlock) = 0;
    virtual void OnOutput(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock) = 0;
};

}