#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace core {

    template<unsigned int BITS> class BigInt {
    protected:
        enum { WIDTH = BITS / 8 };
        uint8_t mData[WIDTH];
    public:
        BigInt() {
            SetZero();
        }
        void SetZero() {
            memset(mData, 0, sizeof(BigInt));
        }
        bool IsZero() const {
            for (int i = 0; i < WIDTH; i++)
                if (mData[i] != 0) return false;
            return true;
        }
    };

}