#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <leveldb/db.h>

namespace compress {

    // uncompress_pubkey and hash
    // compress_pubkey and hash
    class PubKeyDB {
    public:
        constexpr static uint64_t INVALID_INDEX = (uint64_t)-1;

        constexpr static uint64_t UNCOMPRESS_TYPE = 0;
        constexpr static uint64_t COMPRESS_TYPE = 1;
        constexpr static uint64_t UNCOMPRESS_HASH_TYPE = 2;
        constexpr static uint64_t COMPRESS_HASH_TYPE = 3;
    protected:
        leveldb::DB* mDB;
        uint64_t mCount;

        inline static leveldb::Slice getCountKey() {
            return "K";
        }
    public:
        PubKeyDB();
        ~PubKeyDB();

        bool Open(const char *db);
        void Close();

        uint64_t Add(const uint8_t *pubkey, size_t size);
        uint64_t FindHash(const uint8_t *sha256, size_t size);
    };

}