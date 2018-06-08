#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <vector>

namespace compress {

    class Signature {
    protected:
        constexpr static uint8_t COMPRESSED_TAG = 0x04;
        // bit 2 for compress tag
        static bool IsCompressed(uint8_t firstByte) {
            return (firstByte & COMPRESSED_TAG) == COMPRESSED_TAG;
        }
        // Sighash use 0 1 6 7bits
        constexpr static uint8_t SIGHASH_MASK = 0x3C;
        static bool IsSupportSighash(uint8_t sighash) {
            return (sighash & SIGHASH_MASK) == 0;
        }
        // Rlen-29 use 3 4 5bits
        constexpr static uint8_t LENR_MIN = 29;
        constexpr static uint8_t LENR_MAX = 36;
        static bool IsSupportLenR(uint8_t lenR) {
            return LENR_MIN <= lenR && lenR <= LENR_MAX;
        }
    public:
        // Support Format: 0x30 [total-length] 0x02 [R-length] [R] 0x02 [S-length] [S] [sighash]
        // Support Out: [sighash | ((R-length - 29) << 3) | 0x04] [R] [S]
        // Unsupport Out: [0x00, data]
        static bool IsSupport(const uint8_t *sig, size_t size);
        static std::vector<uint8_t> Compress(const uint8_t *sig, size_t size);
        static std::vector<uint8_t> Decompress(const uint8_t *data, size_t size);

        inline static bool IsSupport(const std::vector<uint8_t> &sig) {
            if (sig.empty()) return false;
            return IsSupport(&sig[0], sig.size());
        }
        inline static std::vector<uint8_t> Compress(const std::vector<uint8_t> &sig) {
            if (sig.empty()) return sig;
            return Compress(&sig[0], sig.size());
        }
        inline static std::vector<uint8_t> Decompress(const std::vector<uint8_t> &sig) {
            if (sig.empty()) return sig;
            return Decompress(&sig[0], sig.size());
        }
    };

}
