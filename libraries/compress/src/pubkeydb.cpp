#include <compress/pubkeydb.h>
#include <logging.h>
#include <serialize.h>

namespace compress {

    PubKeyDB::PubKeyDB()
        : mDB(nullptr), mCount(0)
    {}

    PubKeyDB::~PubKeyDB() {
        if (mDB != nullptr)
            delete mDB;
    }

    bool PubKeyDB::Open(const char *db) {
        Close();

        leveldb::Options options;
        options.create_if_missing = true;
        auto status = leveldb::DB::Open(options, db, &mDB);
        if (!status.ok()) {
            LOGE("Open %s(%s)", db, status.ToString().c_str());
            return false;
        }

        std::string result;
        leveldb::Status s = mDB->Get(leveldb::ReadOptions(), getCountKey(), &result);
        if (!status.ok()) {
            Close();
            LOGE("ReadKey %s(%s)", db, status.ToString().c_str());
            return false;
        }

        if (result.size() != sizeof(uint64_t)) {
            mCount = 0;
        } else {
            mCount = core::MemAs<uint64_t>(result.c_str());
        }

        return true;
    }

    void PubKeyDB::Close() {
        if (mDB != nullptr) {
            delete mDB;
            mDB = nullptr;
        }
        mCount = 0;
    }

    uint64_t PubKeyDB::Add(const uint8_t *pubkey, size_t size) {
        return 0;
    }

    uint64_t PubKeyDB::FindHash(const uint8_t *sha256, size_t size) {
        return 0;
    }
}
