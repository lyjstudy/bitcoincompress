#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <vector>

namespace compress {

    class PubKey {
    protected:
        // IsSupport: Decompress(Compress(pubkey)) == pubkey
    public:
        static bool Compress(const uint8_t *pubkey, size_t size, std::vector<uint8_t> &result);
        static bool Decompress(const uint8_t *compressed, size_t size, std::vector<uint8_t> &result);

        inline static bool Compress(const std::vector<uint8_t> &pubkey, std::vector<uint8_t> &result) {
            if (pubkey.empty()) return false;
            return Compress(&pubkey[0], pubkey.size(), result);
        }
        inline static bool Decompress(const std::vector<uint8_t> &compressed, std::vector<uint8_t> &result) {
            if (compressed.empty()) return false;
            return Decompress(&compressed[0], compressed.size(), result);
        }
    };

}
