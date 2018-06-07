#include "compress_amount.h"


CompressAmount::CompressAmount(uint64_t eBits /* 1 => 12 */)
    : mEBits(eBits), mEEnd(1 << eBits), mEMask(((1 << eBits) - 1))
{ }
    
uint64_t CompressAmount::Compress(int64_t amount) {
    if (amount < 0 || amount > 2100000000000000LL) {
        return 0;
    }
}

int64_t Decompress(uint64_t comp) {

}