#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <leveldb/db.h>

namespace compress {

    // uncompress_pubkey and hash
    // compress_pubkey and hash
    constexpr uint64_t INVALID_INDEX = (uint64_t)-1;
    class PubKeyDB {
    public:
        enum class Type {
            Uncompress,
            UncompressHash,
            Compress,
            CompressHash,
        };

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
        uint64_t FindHash(const uint8_t *hash, size_t size);

        inline uint64_t Add(const std::vector<uint8_t> &pubkey) {
            if (pubkey.empty()) return INVALID_INDEX;
            return Add(&pubkey[0], pubkey.size());
        }
        inline uint64_t FindHash(const std::vector<uint8_t> &hash) {
            if (hash.size() != 20) return INVALID_INDEX;
            return FindHash(&hash[0], hash.size());
        }
    };

}