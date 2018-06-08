#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <vector>

namespace compress {

    class PubKey {
    protected:
        // IsSupport: Decompress(Compress(pubkey)) == pubkey
    public:
        static std::vector<uint8_t> Compress(const uint8_t *pubkey, size_t size);
        static std::vector<uint8_t> Decompress(const uint8_t *compressed, size_t size);

        inline static std::vector<uint8_t> Compress(const std::vector<uint8_t> &pubkey) {
            if (pubkey.empty()) return pubkey;
            return Compress(&pubkey[0], pubkey.size());
        }

        inline static std::vector<uint8_t> Decompress(const std::vector<uint8_t> &compressed) {
            if (compressed.empty()) return compressed;
            return Decompress(&compressed[0], compressed.size());
        }
    };

}
