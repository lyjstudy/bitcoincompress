#include <crypto/context.h>
#include <compress/pubkey.h>
#include <bkbase/logging.h>
#include <stdio.h>

namespace compress {

    bool PubKey::Compress(const uint8_t *pubkey, size_t size, std::vector<uint8_t> &result) {
        
        if (size != 65 || pubkey[0] != 0x04) {
            return false;
        }

        result.clear();
        result.insert(result.begin(), pubkey, pubkey + 33);
        result[0] |= (pubkey[64] & 0x01);
        result[0] -= 2;

        return true;
    }

    bool PubKey::Decompress(const uint8_t *pubkey, size_t size, std::vector<uint8_t> &result) {
        
        if (size != 33) {
            return false;
        }

        result.clear();
        result.insert(result.begin(), pubkey, pubkey + 33);

        secp256k1_pubkey secppubkey;
        if (!secp256k1_ec_pubkey_parse(crypto::Secp256k1, &secppubkey, &result[0], result.size())) {
            return false;
        }

        result.resize(65);
        size_t publen = 65;
        secp256k1_ec_pubkey_serialize(crypto::Secp256k1, &result[0], &publen, &secppubkey, SECP256K1_EC_UNCOMPRESSED);

        result.resize(publen);

        return true;
    }
}
