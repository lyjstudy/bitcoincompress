#include <compress/pubkeydb.h>
#include <compress/pubkey.h>
#include <bkbase/logging.h>
#include <bkbase/serialize.h>
#include <bkbase/hash.h>

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
            BKERROR() << "Open " << db << "(" << status.ToString() << ")";
            return false;
        }

        std::string result;
        leveldb::Status s = mDB->Get(leveldb::ReadOptions(), getCountKey(), &result);
        if (!status.ok()) {
            Close();
            BKERROR() << "ReadKey " << db << "(" << status.ToString() << ")";
            return false;
        }

        if (result.size() != sizeof(uint64_t)) {
            mCount = 0;
        } else {
            mCount = bkbase::ReadLE<uint64_t>(result.c_str());
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
        uint8_t uncompress[65];
        uint8_t compress[33];
        if (size == 65) {
            memcpy(uncompress, pubkey, 65);
            auto vcompress = PubKey::Compress(uncompress, 65);
            if (vcompress.size() != 33) return INVALID_INDEX;
            memcpy(compress, &vcompress[0], 33);
        } else if (size == 33) {
            memcpy(compress, pubkey, 33);
            auto vuncompress = PubKey::Decompress(compress, 33);
            if (vuncompress.size() != 65) return INVALID_INDEX;
            memcpy(uncompress, &vuncompress[0], 65);
        } else {
            return INVALID_INDEX;
        }
        //core::Uint160 compressHash = core::HashShaRipemd160((const char *)compress, sizeof(compress));
        //core::Uint160 uncompressHash = core::HashShaRipemd160((const char *)uncompress, sizeof(uncompress));
        
        leveldb::Slice key((const char *)pubkey, size);
        return 0;
    }

    uint64_t PubKeyDB::FindHash(const uint8_t *hash, size_t size) {
        return 0;
    }
}
