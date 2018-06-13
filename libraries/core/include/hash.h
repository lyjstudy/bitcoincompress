#pragma once

#include <stdint.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <memory>
#include "hex.h"
#include "serialize.h"

namespace core {

    template<unsigned int BITS, typename Allocator = std::allocator<uint8_t>>
    class HashBase {
    protected:
        enum { WIDTH = BITS / 8 };
        uint8_t *mDataPtr;

        static uint8_t ZERO_DATA[WIDTH];
        static Allocator allocator;

    public:
        HashBase()
            : mDataPtr(nullptr) {
        }
        ~HashBase() {
            if (mDataPtr != nullptr)
                allocator.deallocate(mDataPtr, Size());
        }
        HashBase(const HashBase &other) {
            if (other.mDataPtr == nullptr) {
                mDataPtr = nullptr;
            } else {
                mDataPtr = allocator.allocate(Size());
                memcpy(mDataPtr, other.mDataPtr, Size());
            }
        }
        HashBase(HashBase &&other)  {
            mDataPtr = other.mDataPtr;
            other.mDataPtr = nullptr;
        }
        inline void SetZero() {
            if (mDataPtr != nullptr)
                memset(mDataPtr, 0, Size());
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
        inline std::string GetHex() const {
            if (mDataPtr == nullptr) {
                return HexFromReverseBin(ZERO_DATA, Size());
            }
            return HexFromReverseBin(mDataPtr, Size());
        }
        void SetHex(const char *psz) {
            if (mDataPtr == nullptr) mDataPtr = allocator.allocate(Size());
            memset(mDataPtr, 0, Size());
            HexToReverseBin(psz, mDataPtr, Size());
        }
        void SetData(const std::vector<uint8_t> &data) {
            if (mDataPtr == nullptr) mDataPtr = allocator.allocate(Size());
            memset(mDataPtr, 0, Size());
            if (!data.empty())
                memcpy(mDataPtr, &data[0], std::min(data.size(), Size()));
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
        template <typename Stream> bool Unserialize(Stream &s) {
            if (mDataPtr == nullptr) mDataPtr = allocator.allocate(Size());
            return s.Read((char *)mDataPtr, Size()) == Size();
        }

        static HashBase<BITS, Allocator> From(const char *str) {
            HashBase<BITS, Allocator> rv;
            rv.SetHex(str);
            return rv;
        }

        uint64_t GetCheapHash() const {
            static_assert(BITS >= 64, "HashBase BITS too small");
            if (mDataPtr == nullptr) return 0;
            return MemAs<uint64_t>(mDataPtr);
        }
    };

    template<unsigned int BITS, typename Allocator>
    uint8_t HashBase<BITS, Allocator>::ZERO_DATA[WIDTH] = {0};

    template<unsigned int BITS, typename Allocator>
    Allocator HashBase<BITS, Allocator>::allocator;

    class Uint160 : public HashBase<160> {
    public:
        using HashBase<160>::HashBase;
    };

    class Uint256 : public HashBase<256> {
    public:
        using HashBase<256>::HashBase;
    };

    template<typename Hash>
    struct HashOperator {
        size_t operator() (const Hash &hash) const { return hash.GetCheapHash(); }
    };

    Uint256 HashSha256(const char *mem, size_t size);
    Uint160 Ripemd160(const char *mem, size_t size);
    Uint256 HashDoubleSha256(const char *mem, size_t size);
    Uint160 HashShaRipemd160(const char *mem, size_t size);

    inline Uint256 HashDoubleSha256(const std::vector<uint8_t> &buf) {
        if (buf.empty()) return Uint256();
        return HashDoubleSha256((const char *)&buf[0], buf.size());
    }
    inline Uint160 HashShaRipemd160(const std::vector<uint8_t> &buf) {
        if (buf.empty()) return Uint160();
        return HashShaRipemd160((const char *)&buf[0], buf.size());
    }
}