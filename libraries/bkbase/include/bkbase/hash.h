#pragma once

#include <stdint.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <memory>
#include "hex.h"
#include "endian.h"
#include "serialize.h"

namespace bkbase
{

template<unsigned int BITS, typename Allocator = std::allocator<uint8_t>>
class HashBase {
public:
    enum { WIDTH = BITS / 8 };
protected:
    uint8_t *mDataPtr;

    static uint8_t ZERO_DATA[WIDTH];
    static Allocator allocator;

public:
    HashBase()
        : mDataPtr(nullptr) {
    }
    HashBase(const HashBase &other) : mDataPtr(nullptr) {
        if (other.mDataPtr != nullptr) {
            mDataPtr = allocator.allocate(Size());
            memcpy(mDataPtr, other.mDataPtr, Size());
        }
    }
    HashBase(HashBase &&other) : mDataPtr(other.mDataPtr)  {
        other.mDataPtr = nullptr;
    }
    HashBase(const std::vector<uint8_t> &data)
        : mDataPtr(allocator.allocate(Size())) {
        memset(mDataPtr, 0, Size());
        if (!data.empty())
            memcpy(mDataPtr, &data[0], data.size() < Size() ? data.size() : Size());
    }
    HashBase(const void *ptr, size_t size)
        : mDataPtr(allocator.allocate(Size())) {
        memset(mDataPtr, 0, Size());
        memcpy(mDataPtr, ptr, size < Size() ? size : Size());
    }
    ~HashBase() {
        if (mDataPtr != nullptr)
            allocator.deallocate(mDataPtr, Size());
    }
    inline void SetZero() {
        if (mDataPtr != nullptr)
            memset(mDataPtr, 0, Size());
    }
    bool hasMemory() const {
        return mDataPtr != nullptr;
    }
    bool IsZero() const {
        if (mDataPtr == nullptr) return true;

        return std::any_of(mDataPtr, mDataPtr + Size(), [](const uint8_t value) {
            return value != 0;
        }) == false;
    }
    int Compare(const HashBase &other) const {
        if (IsZero())
            return other.IsZero() ? 0 : -1;
        if (other.IsZero())
            return 1;
        return memcmp(mDataPtr, other.mDataPtr, Size());
    }
    inline std::string GetReverseHex() const {
        if (mDataPtr == nullptr) {
            return HexFromReverseBin(ZERO_DATA, Size());
        }
        return HexFromReverseBin(mDataPtr, Size());
    }
    inline std::string GetHex() const {
        if (mDataPtr == nullptr) {
            return HexFromBin(ZERO_DATA, Size());
        }
        return HexFromBin(mDataPtr, Size());
    }
    void SetReverseHex(const char *psz) {
        if (mDataPtr == nullptr) mDataPtr = allocator.allocate(Size());
        memset(mDataPtr, 0, Size());
        HexToReverseBin(psz, mDataPtr, Size());
    }
    void SetHex(const char *psz) {
        if (mDataPtr == nullptr) mDataPtr = allocator.allocate(Size());
        memset(mDataPtr, 0, Size());
        HexToBin(psz, mDataPtr, Size());
    }
    void SetData(const std::vector<uint8_t> &data) {
        if (mDataPtr == nullptr) mDataPtr = allocator.allocate(Size());
        memset(mDataPtr, 0, Size());
        if (!data.empty())
            memcpy(mDataPtr, &data[0], data.size() < Size() ? data.size() : Size());
    }

    friend inline bool operator==(const HashBase &a, const HashBase &b) {
        return a.Compare(b) == 0;
    }
    friend inline bool operator!=(const HashBase &a, const HashBase &b) {
        return a.Compare(b) != 0;
    }
    friend inline bool operator<(const HashBase &a, const HashBase &b) {
        return a.Compare(b) < 0;
    }
    HashBase &operator=(HashBase &&other) {
        if (mDataPtr != nullptr) allocator.deallocate(mDataPtr, Size());
        mDataPtr = other.mDataPtr;
        other.mDataPtr = nullptr;
        return *this;
    }

    inline unsigned int Size() const { return WIDTH; }
    inline unsigned int size() const { return Size(); }

    const uint8_t *begin() const {
        return mDataPtr == nullptr ? ZERO_DATA : mDataPtr;
    }
    const uint8_t *end() const {
        return begin() + Size();
    }
    uint8_t *begin() {
        if (mDataPtr == nullptr) mDataPtr = allocator.allocate(Size());
        return mDataPtr;
    }
    uint8_t *end() {
        return begin() + Size();
    }

    template <typename Stream> void Serialize(Stream &s) const {
        if (mDataPtr == nullptr) s.Write((const char *)ZERO_DATA, Size());
        else s.Write((const char *)mDataPtr, Size());
    }
    template <typename Stream> void Unserialize(Stream &s) {
        if (mDataPtr == nullptr) mDataPtr = allocator.allocate(Size());
        if (s.Read((char *)mDataPtr, Size()) != Size()) {
            SerializeThrow("Unserialize HashBase failure");
        }
    }

    template<typename T>
    static T From(const char *str) {
        T rv;
        rv.SetReverseHex(str);
        return rv;
    }

    uint64_t GetCheapHash() const {
        static_assert(BITS >= 64, "HashBase BITS too small");
        if (mDataPtr == nullptr) return 0;
        return ReadLE<uint64_t>(mDataPtr);
    }
};

template<unsigned int BITS, typename Allocator>
uint8_t HashBase<BITS, Allocator>::ZERO_DATA[WIDTH] = {0};

template<unsigned int BITS, typename Allocator>
Allocator HashBase<BITS, Allocator>::allocator;


class Hash160 : public HashBase<160> {
public:
    using HashBase<160>::HashBase;
};

class Hash256 : public HashBase<256> {
public:
    using HashBase<256>::HashBase;
};

struct HashOperator {
    template<typename Hash>
    size_t operator() (const Hash &hash) const { return hash.GetCheapHash(); }
};


} // bkbase
