#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <vector>

namespace compress {

    
    bool SignatureCompressible(const uint8_t *sig, size_t size);

    class Signature {
    protected:
        // Support Format: 0x30 [total-length] 0x02 [R-length] [R] 0x02 [S-length] [S] [sighash]
        // Support Out: [signash | ((R-length - 29) << 3) | 0x04] [R] [S]
        static bool IsSupport(const uint8_t *sig, size_t size);
    public:
        static std::vector<uint8_t> Compress(const uint8_t *sig, size_t size);
        static std::vector<uint8_t> Decompress(const uint8_t *data, size_t size);
    };

}
