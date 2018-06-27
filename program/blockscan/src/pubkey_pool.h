#pragma once

#include <helper/scan.h>
#include <helper/output_pool.h>
#include <script/opcode.h>
#include <script/type.h>
#include <compress/pubkeydb.h>
#include <atomic>
#include <iomanip>

class PubKeyPool : public helper::OutputPool {
protected:
    compress::PubKeyDB mDB;

public:
    PubKeyPool(const char *db) {
        if (!mDB.Open(db)) {
            exit(-1);
        }
    }
    virtual void OnTransaction(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock, const std::vector<uint8_t> &unlock, int64_t amount) {
        std::vector<std::vector<uint8_t>> address;
        std::vector<std::vector<uint8_t>> inputAddress;
        auto type = script::GetOutputType(lock, &address);
        switch (type) {
        case script::Type::ScriptHash:
            switch (script::GetInputType(unlock, type, &inputAddress)) {
                case script::Type::PubKey:
                    if (!mDB.Exists(address.back()))
                        mDB.Add(address.back());
                    break;
                case script::Type::PubKeyHash:
                    if (!mDB.Exists(inputAddress.back()))
                        mDB.Add(inputAddress.back());
                    break;
                case script::Type::MultiSig:
                    for (size_t i = 2; i < inputAddress.size(); i++) {
                        if (!mDB.Exists(inputAddress[i]))
                            mDB.Add(inputAddress[i]);
                    }
                    break;
                default:
                    break;
            }
            break;
        case script::Type::PubKey:
            if (!mDB.Exists(address.back()))
                mDB.Add(address.back());
            break;
        case script::Type::MultiSig:
            for (size_t i = 2; i < address.size(); i++) {
                if (!mDB.Exists(address[i]))
                    mDB.Add(address[i]);
            }
            break;
        case script::Type::PubKeyHash:
            if (script::GetInputType(unlock, type, &inputAddress) == type) {
                if (!mDB.Exists(inputAddress.back()))
                    mDB.Add(inputAddress.back());
            }
            break;
        default:
            break;
        }
    }
    // Main Thread
    virtual void OnOutput(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock, int64_t amount) {
        std::vector<std::vector<uint8_t>> address;
        auto type = script::GetOutputType(lock, &address);
        switch (type) {
        case script::Type::PubKey:
            if (!mDB.Exists(address.back()))
                mDB.Add(address.back());
            break;
        case script::Type::MultiSig:
            for (size_t i = 2; i < address.size(); i++) {
                if (!mDB.Exists(address[i]))
                    mDB.Add(address[i]);
            }
            break;
        default:
            break;
        }
    }
};
