#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <leveldb/db.h>
#include <bkbase/logging.h>
#include <bkbase/serialize.h>
#include <bkbase/hash.h>

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
        inline static Type dbGetType(const std::string &value) {
            return Type(bkbase::ReadLE<uint64_t>(value.c_str()) & 0x03);
        }
        inline static uint64_t dbGetIndex(const std::string &value) {
            return bkbase::ReadLE<uint64_t>(value.c_str()) >> 2;
        }
        inline static leveldb::Slice dbMakeValue(uint64_t index, Type type) {
            char keyBuf[sizeof(uint64_t)];
            bkbase::WriteLE<uint64_t>(keyBuf, (index << 2) | (uint64_t)type);
            return leveldb::Slice(keyBuf, sizeof(uint64_t));
        }
    public:
        PubKeyDB();
        ~PubKeyDB();

        bool Open(const char *db);
        void Close();

        uint64_t Add(const uint8_t *pubkey, size_t size);
        inline uint64_t Add(const std::vector<uint8_t> &pubkey) {
            if (pubkey.empty()) return INVALID_INDEX;
            return Add(&pubkey[0], pubkey.size());
        }

        uint64_t Find(const uint8_t *data, size_t size);
        inline uint64_t Find(const std::vector<uint8_t> &data) {
            if (data.empty()) return INVALID_INDEX;
            return Find(&data[0], data.size());
        }
    };

}