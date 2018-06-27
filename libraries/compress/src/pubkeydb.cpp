#include <compress/pubkeydb.h>
#include <compress/pubkey.h>
#include <crypto/hasher.h>

namespace compress {

    PubKeyDB::PubKeyDB()
        : mDB(nullptr), mCount(0)
    {}

    PubKeyDB::~PubKeyDB() {
        Close();
    }

    bool PubKeyDB::Open(const char *db) {
        Close();

        leveldb::Options options;
        options.create_if_missing = true;
        auto status = leveldb::DB::Open(options, db, &mDB);
        if (!status.ok()) {
            BKERROR() << "leveldb::DB::Open" << "(" << db << "): " << status.ToString();
            return false;
        }

        std::string result;
        status = mDB->Get(leveldb::ReadOptions(), getCountKey(), &result);
        if (!status.ok()) {
            if (!status.IsNotFound()) {
                BKERROR() << "leveldb::DB::Get(K): " << status.ToString();
                Close();
                return false;
            }
            return true;
        }

        if (result.size() != sizeof(uint64_t)) {
            mCount = 0;
        } else {
            mCount = bkbase::ReadLE<uint64_t>(result.c_str());
        }

        BKINFO() << "PubKeyDB Count: " << mCount;

        return true;
    }

    void PubKeyDB::Close() {
        if (mDB != nullptr) {
            char keyBuf[sizeof(uint64_t)];
            bkbase::WriteLE<uint64_t>(keyBuf, mCount);
            leveldb::Slice value(keyBuf, sizeof(uint64_t));
            auto status = mDB->Put(leveldb::WriteOptions(), getCountKey(), value);
            if (!status.ok()) {
                BKERROR() << "leveldb::DB::Put(K): " << status.ToString();
            }
            delete mDB;
            mDB = nullptr;
        }
        mCount = 0;
    }

    uint64_t PubKeyDB::Add(const uint8_t *pubkey, size_t size) {
        uint8_t uncompress[65];
        uint8_t compress[33];
        bool isCompressed = false;
        if (size == 65) {
            memcpy(uncompress, pubkey, 65);
            std::vector<uint8_t> vcompress;
            if (!PubKey::Compress(uncompress, 65, vcompress)) return INVALID_INDEX;
            if (vcompress.size() != 33) return INVALID_INDEX;
            memcpy(compress, &vcompress[0], 33);
            // test
            std::vector<uint8_t> vuncompress;
            if (!PubKey::Decompress(compress, 33, vuncompress)) return INVALID_INDEX;
            if (vuncompress.size() != 65) return INVALID_INDEX;
            if (memcmp(&vuncompress[0], uncompress, 65) != 0) return INVALID_INDEX;
        } else if (size == 33) {
            isCompressed = true;
            memcpy(compress, pubkey, 33);
            std::vector<uint8_t> vuncompress;
            if (!PubKey::Decompress(compress, 33, vuncompress)) return INVALID_INDEX;
            if (vuncompress.size() != 65) return INVALID_INDEX;
            memcpy(uncompress, &vuncompress[0], 65);
        } else {
            return INVALID_INDEX;
        }

        bkbase::Hash160 compressHash = crypto::CHash160().Write(compress, sizeof(compress)).Finalize();
        bkbase::Hash160 uncompressHash = crypto::CHash160().Write(uncompress, sizeof(uncompress)).Finalize();

        leveldb::Slice keys[4] = {
            leveldb::Slice((char *)compress, sizeof(compress)),
            leveldb::Slice((char *)uncompress, sizeof(uncompress)),
            leveldb::Slice((char *)compressHash.begin(), compressHash.size()),
            leveldb::Slice((char *)uncompressHash.begin(), uncompressHash.size()),
        };

        uint64_t curIndex = mCount++;

        if (curIndex % 100000 == 0) {
            BKINFO() << "Pubkey: " << curIndex;
        }

        leveldb::Slice values[4] = {
            dbMakeValue(curIndex, Type::Compress),
            dbMakeValue(curIndex, Type::Uncompress),
            dbMakeValue(curIndex, Type::CompressHash),
            dbMakeValue(curIndex, Type::UncompressHash),
        };
        
        for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
            auto status = mDB->Put(leveldb::WriteOptions(), keys[i], values[i]);
            if (!status.ok()) {
                BKERROR() << "leveldb::DB::Write" << "(" << curIndex << "," << i << "): " << status.ToString();
            }
        }

        return bkbase::ReadLE<uint64_t>(values[isCompressed ? 0 : 1].data());
    }

    uint64_t PubKeyDB::Find(const uint8_t *data, size_t size) {

        Type guessType;

        if (size == 20) guessType = Type::UncompressHash;
        else if (size == 33) guessType = Type::Compress;
        else if (size == 65) guessType = Type::Uncompress;
        else return INVALID_INDEX;

        leveldb::Slice key((char *)data, size);
        std::string result;

        auto status = mDB->Get(leveldb::ReadOptions(), key, &result);
        if (!status.ok()) {
            return INVALID_INDEX;
        }
        if (result.size() != sizeof(uint64_t)) {
            return INVALID_INDEX;
        }

        switch (guessType) {
            case Type::Compress:
                if (dbGetType(result) == Type::Compress)
                    return bkbase::ReadLE<uint64_t>(result.c_str());
                break;
            case Type::Uncompress:
                if (dbGetType(result) == Type::Uncompress)
                    return bkbase::ReadLE<uint64_t>(result.c_str());
                break;
            case Type::UncompressHash:
                if (dbGetType(result) == Type::UncompressHash || dbGetType(result) == Type::CompressHash)
                    return bkbase::ReadLE<uint64_t>(result.c_str());
                break;
            default:
                break;
        }

        // BKERROR() << bkbase::HexFromBin(data, size) << " guess:" << (int)guessType << " real:" << (int)dbGetType(result);

        return INVALID_INDEX;
    }
}
