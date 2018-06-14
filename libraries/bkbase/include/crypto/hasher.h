#pragma once


#include <vector>
#include <openssl/evp.h>
#include <bkbase/logging.h>
#include <bkbase/hash.h>
#include <bkbase/stream.h>

namespace crypto {

#define OPENSSL_ERROR_CALL(api, ...) { \
    int api_result = api(__VA_ARGS__); \
    if (api_result != 1) { \
        BKERROR() << #api << ": " << api_result; \
    } \
}
template<typename RESULT, decltype(EVP_sha256) FUNC>
class CHasher {
private:
    EVP_MD_CTX *mContext;
public:
    static const size_t OUTPUT_SIZE = RESULT::WIDTH;
    using OUTPUT_TYPE = RESULT;

    explicit CHasher() : mContext(EVP_MD_CTX_new()) {
        OPENSSL_ERROR_CALL(EVP_DigestInit, mContext, FUNC());
    }
    ~CHasher() {
        EVP_MD_CTX_free(mContext);
    }
    CHasher &Write(const void *data, size_t size) {
        OPENSSL_ERROR_CALL(EVP_DigestUpdate, mContext, data, size);
        return *this;
    }
    template<typename T, typename A>
    CHasher &Write(const std::vector<T, A> &buffer) {
        if (buffer.empty()) return *this;
        OPENSSL_ERROR_CALL(EVP_DigestUpdate, mContext, &buffer[0], buffer.size());
        return *this;
    }
    OUTPUT_TYPE Finalize() {
        OUTPUT_TYPE result;
        OPENSSL_ERROR_CALL(EVP_DigestFinal, mContext, result.begin(), nullptr);
        return std::move(result);
    }
    void Finalize(uint8_t output[OUTPUT_SIZE]) {
        OPENSSL_ERROR_CALL(EVP_DigestFinal, mContext, output, nullptr);
    }
    CHasher &Reset() {
        OPENSSL_ERROR_CALL(EVP_DigestInit, mContext, FUNC());
        return *this;
    }
};
template<typename FIRST, typename SECOND>
class CDoubleHasher {
private:
    FIRST mFirstHasher;
public:
    static const size_t OUTPUT_SIZE = SECOND::OUTPUT_SIZE;
    using OUTPUT_TYPE = typename SECOND::OUTPUT_TYPE;

    template<typename T, typename A>
    CDoubleHasher &Write(const std::vector<T, A> &buffer) {
        mFirstHasher.Write(buffer);
        return *this;
    }
    CDoubleHasher &Write(const void *data, size_t size) {
        mFirstHasher.Write(data, size);
        return *this;
    }
    OUTPUT_TYPE Finalize() {
        uint8_t firstValue[FIRST::OUTPUT_SIZE];
        mFirstHasher.Finalize(firstValue);
        SECOND second;
        second.Write(firstValue, FIRST::OUTPUT_SIZE);
        return second.Finalize();
    }
    void Finalize(uint8_t output[OUTPUT_SIZE]) {
        uint8_t firstValue[FIRST::OUTPUT_SIZE];
        mFirstHasher.Finalize(firstValue);
        SECOND second;
        second.Write(firstValue, FIRST::OUTPUT_SIZE);
        second.Finalize(output);
    }
    CDoubleHasher &Reset() {
        mFirstHasher.Reset();
        return *this;
    }
};

using CSHA256Hasher = CHasher<bkbase::Hash256, EVP_sha256>;
using CRIPEMD160Hasher = CHasher<bkbase::Hash160, EVP_ripemd160>;
using CHash256 = CDoubleHasher<CSHA256Hasher, CSHA256Hasher>;
using CHash160 = CDoubleHasher<CSHA256Hasher, CRIPEMD160Hasher>;

template<typename HASHER>
class StreamHash : public bkbase::StreamBase {
protected:
    HASHER mHasher;
public:
    using OUTPUT_TYPE = typename HASHER::OUTPUT_TYPE;
    explicit StreamHash(int type, int version)
        : bkbase::StreamBase(type, version) {}

    inline void Write(const char *buf, size_t size) {
        mHasher.Write(buf, size);
    }
    inline size_t Size() const { return HASHER::OUTPUT_SIZE; }
    inline void Reserve(size_t) {}
    inline void Clear() { mHasher.Reset(); }

    template <typename T> inline StreamHash &operator<<(const T &obj) {
        ::bkbase::Serialize(*this, obj);
        return (*this);
    }
    inline OUTPUT_TYPE GetHash() {
        return mHasher.Finalize();
    }
};

using StreamHash256 = StreamHash<CHash256>;


}