#pragma once

#include <stdint.h>

namespace compress {

    /*
        amount 0=>2100 0000 0000 0000

        compressed:
            base = amount / (10 ^ exponent)
            compressed = (base << (exponent_bits + 1)) | (exponent << 1) | 1
        uncompressed:
            amount = amount << 1
    */
    template<unsigned int EXPONENT_BITS>
    class AmountBase {
    protected:
        constexpr static unsigned int EXPONENT_END = 1 << EXPONENT_BITS;
        constexpr static unsigned int EXPONENT_MASK = EXPONENT_END - 1;
    public:
        constexpr static int64_t MAX_VALUE = 2100000000000000LL;

        static uint64_t Compress(int64_t amount) {
            static_assert(1 <= EXPONENT_BITS && EXPONENT_BITS <= 12, "1 <= EXPONENT_BITS <= 12");
            if (amount < 0 || amount > MAX_VALUE) return 0;
            unsigned int e = 0;
            uint64_t base = (uint64_t)amount;
            while ((base % 10) == 0 && e < EXPONENT_END) {
                base /= 10;
                e++;
            }

            base <<= (EXPONENT_BITS + 1);
            base |= (e << 1);
            base |= 1;

            if (base >= (uint64_t)amount) return (uint64_t)amount << 1;

            return base;
        }

        static int64_t Decompress(uint64_t comp) {
            static_assert(1 <= EXPONENT_BITS && EXPONENT_BITS <= 12, "1 <= EXPONENT_BITS <= 12");
            if ((comp & 1) == 0) return (int64_t)(comp >> 1);

            comp >>= 1;
            unsigned int e = comp & EXPONENT_MASK;
            comp >>= EXPONENT_BITS;
            comp *= 10 ^ e;

            return comp;
        }
    };

    using Amount = AmountBase<4>;
}
