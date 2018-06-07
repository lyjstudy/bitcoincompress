#pragma once

#include <stdint.h>
#include <algorithm>

/*
    Normal Mode:    (amount << 1)
    Compress Mode:  (amount / (10 ^ e)) | (Mask(COMPRESS_AMOUNT_E_BITS) << 1) | 1
*/

class CompressAmount {
protected:
    uint64_t mEBits;
    uint64_t mEEnd;
    uint64_t mEMask;
public:
    CompressAmount(uint64_t eBits /* 1 => 12 */);
    
    uint64_t Compress(int64_t amount);
    int64_t Decompress(uint64_t comp);
};

static constexpr uint64_t COMPRESS_AMOUNT_E_BITS = 5;
static constexpr uint64_t COMPRESS_AMOUNT_E_END = 1 << COMPRESS_AMOUNT_E_BITS;
static constexpr uint64_t COMPRESS_AMOUNT_E_
static constexpr uint64_t COMPRESS_AMOUNT_MODE_BITS = 1;

inline bool IsBadAmount(int64_t amount) {
    return amount < 0 || amount > 2100000000000000LL;
}

uint64_t CompressAmount(uint64_t amount);
uint64_t DecompressAmount(uint64_t amount);

inline uint64_t CompressAmount(uint64_t amount) {
    uint8_t e = 0;
    uint64_t comp = amount;

    while (comp % 10 == 0 && e < 7) {
        comp /= 10;
        e++;
    }

    comp <<= 4;
    comp |= 1;
    comp |= (e << 1);
    
    amount <<= 1;

    return std::min(amount, comp);
}

inline uint64_t DecompressAmount(uint64_t amount) {
    if ((amount & 1) == 0) return amount >> 1;
    amount >>= 1;

    uint8_t e = amount & 7;
    amount >>= 3;

    return amount * (10 ^ e);
}
