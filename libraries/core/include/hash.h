#pragma once

#include <stdint.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include "hex.h"
#include "serialize.h"

namespace core {

    template<unsigned int BITS>
    class HashBase {
    protected:
        enum { WIDTH = BITS / 8 };
        uint8_t mData[WIDTH];
    public:
        HashBase() {
            SetZero();
        }
        explicit HashBase(const std::vector<uint8_t> &data) {
            memcpy(mData, &data[0], std::min(data.size(), sizeof(mData)));
        }

        inline void SetZero() {
            memset(mData, 0, sizeof(mData));
        }
        bool IsZero() const {
            return std::any_of(begin(), end(), [](const uint8_t value) {
                return value != 0;
            }) == false;
        }

        int Compare(const HashBase &other) const {
            return memcmp(mData, other.mData, sizeof(mData));
        }

        inline std::string GetHex() const {
            return HexFromReverseBin(mData, sizeof(mData));
        }
        void SetHex(const char *psz) {
            SetZero();
            HexToReverseBin(psz, mData, sizeof(mData));
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

        inline uint8_t *begin() { return &mData[0]; }
        inline uint8_t *end() { return &mData[WIDTH]; }
        inline const uint8_t *begin() const { return &mData[0]; }
        inline const uint8_t *end() const { return &mData[WIDTH]; }
        inline unsigned int size() const { return sizeof(mData); }

        template <typename Stream> void Serialize(Stream &s) const {
            s.Write((const char *)mData, sizeof(mData));
        }
        template <typename Stream> bool Unserialize(Stream &s) {
            return s.Read((char *)mData, sizeof(mData)) == sizeof(mData);
        }
    };

    class Uint160 : public HashBase<160> {
    public:
        Uint160() {}
        Uint160(const HashBase<160> &b) : HashBase<160>(b) {}
        explicit Uint160(const std::vector<uint8_t> &vch) : HashBase<160>(vch) {}

        inline static Uint160 From(const char *str) {
            Uint160 rv;
            rv.SetHex(str);
            return rv;
        }
    };

    class Uint256 : public HashBase<256> {
    public:
        Uint256() {}
        Uint256(const HashBase<256> &b) : HashBase<256>(b) {}
        explicit Uint256(const std::vector<uint8_t> &vch) : HashBase<256>(vch) {}

        inline static Uint256 From(const char *str) {
            Uint256 rv;
            rv.SetHex(str);
            return rv;
        }
        /**
         * A cheap hash function that just returns 64 bits from the result, it can
         * be used when the contents are considered uniformly random. It is not
         * appropriate when the value can easily be influenced from outside as e.g.
         * a network adversary could provide values to trigger worst-case behavior.
         */
        uint64_t GetCheapHash() const { return MemAs<uint64_t>(mData); }
    };
    
    struct Uint256Operator {
        size_t operator()(const Uint256 &hash) const { return hash.GetCheapHash(); }
    };

    Uint256 HashSha256(const char *mem, size_t size);
    Uint160 Ripemd160(const char *mem, size_t size);
    Uint256 HashDoubleSha256(const char *mem, size_t size);
}