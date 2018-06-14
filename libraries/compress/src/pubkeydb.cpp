#include <compress/pubkeydb.h>
#include <compress/pubkey.h>
#include <crypto/hasher.h>

namespace compress {

#define LEVELDB_NO_RETURN
#define LEVELDB_RETURN_FALSE Close(); return false
#define LEVELDB_NOT_FOUND_FALSE if (!status.IsNotFound()) { Close(); return false; }

#define __LEVELDB_CHECK_CALL(api, ret, ...) { \
    auto status = api(__VA_ARGS__); \
    if (!status.ok()) { \
        BKERROR() << #api << "(" << #__VA_ARGS__ << "): " << status.ToString(); \
        ret; \
    } \
}

#define LEVELDB_CHECK_CALL(api, ...) __LEVELDB_CHECK_CALL(api, LEVELDB_RETURN_FALSE, __VA_ARGS__)
#define LEVELDB_CHECK_CALL_VOID(api, ...) __LEVELDB_CHECK_CALL(api, LEVELDB_NO_RETURN, __VA_ARGS__)
#define LEVELDB_CHECK_CALL_NOT_FOUND(api, ...) __LEVELDB_CHECK_CALL(api, LEVELDB_NOT_FOUND_FALSE, __VA_ARGS__)

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
        LEVELDB_CHECK_CALL(leveldb::DB::Open, options, db, &mDB);

        std::string result;
        LEVELDB_CHECK_CALL_NOT_FOUND(mDB->Get, leveldb::ReadOptions(), getCountKey(), &result);


        if (result.size() != sizeof(uint64_t)) {
            mCount = 0;
        } else {
            mCount = bkbase::ReadLE<uint64_t>(result.c_str());
        }

        return true;
    }

    void PubKeyDB::Close() {
        if (mDB != nullptr) {
            char keyBuf[sizeof(uint64_t)];
            bkbase::WriteLE<uint64_t>(keyBuf, mCount);
            leveldb::Slice value(keyBuf, sizeof(uint64_t));
            LEVELDB_CHECK_CALL_VOID(mDB->Put, leveldb::WriteOptions(), getCountKey(), value);
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
            auto vcompress = PubKey::Compress(uncompress, 65);
            if (vcompress.size() != 33) return INVALID_INDEX;
            memcpy(compress, &vcompress[0], 33);
        } else if (size == 33) {
            isCompressed = true;
            memcpy(compress, pubkey, 33);
            auto vuncompress = PubKey::Decompress(compress, 33);
            if (vuncompress.size() != 65) return INVALID_INDEX;
            memcpy(uncompress, &vuncompress[0], 65);
        } else {
            return INVALID_INDEX;
        }
        bkbase::Hash160 compressHash = crypto::CHash160().Write(compress, sizeof(compress)).Finalize();
        bkbase::Hash160 uncompressHash = crypto::CHash160().Write(uncompress, sizeof(uncompress)).Finalize();

/*
        leveldb::Slice keyCompress((char *)compress, sizeof(compress));
        leveldb::Slice keyUncompress((char *)uncompress, sizeof(uncompress));
        leveldb::Slice keyCompressHash((char *)compressHash.begin(), compressHash.size());
        leveldb::Slice keyUncompressHash((char *)uncompressHash.begin(), uncompressHash.size());
*/
        leveldb::Slice keys[4] = {
            leveldb::Slice((char *)compress, sizeof(compress)),
            leveldb::Slice((char *)uncompress, sizeof(uncompress)),
            leveldb::Slice((char *)compressHash.begin(), compressHash.size()),
            leveldb::Slice((char *)uncompressHash.begin(), uncompressHash.size()),
        };

#if 1
        BKWARN() << "Duplicate Checking";
        std::string result;
        for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
            result.clear();
            auto status = mDB->Get(leveldb::ReadOptions(), keys[i], &result);
            if (!status.IsNotFound()) {
                BKCRITICAL() << bkbase::HexFromBin((uint8_t *)keys[i].data(), keys[i].size())
                    << ":" << bkbase::HexFromBin((uint8_t *)result.c_str(), result.size())
                    << " Duplicated";
            }
        }
#endif
        uint64_t curIndex = mCount++;

        leveldb::Slice values[4] = {
            dbMakeValue(curIndex, Type::Compress),
            dbMakeValue(curIndex, Type::Uncompress),
            dbMakeValue(curIndex, Type::CompressHash),
            dbMakeValue(curIndex, Type::UncompressHash),
        };
        
        for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
            LEVELDB_CHECK_CALL_VOID(mDB->Put, leveldb::WriteOptions(), keys[i], values[i]);
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
            BKINFO() << bkbase::HexFromBin(data, size) << ": " << status.ToString();
            return INVALID_INDEX;
        }
        if (result.size() != sizeof(uint64_t)) {
            BKERROR() << bkbase::HexFromBin(data, size) << ": " 
                << bkbase::HexFromBin((uint8_t*)result.c_str(), result.size());
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

        BKERROR() << bkbase::HexFromBin(data, size) << " guess:" << (int)guessType << " real:" << (int)dbGetType(result);

        return INVALID_INDEX;
    }
}
