#include <crypto/context.h>
#include <compress/pubkey.h>
#include <bkbase/logging.h>
#include <stdio.h>

namespace compress {

    std::vector<uint8_t> PubKey::Compress(const uint8_t *pubkey, size_t size) {
        
        if (size != 65 || pubkey[0] != 0x04) {
            BKERROR() << "Invalid pubkey size:" << size
                         << " or first:" << (size > 0 ? (int)pubkey[0] : -1);
            return std::vector<uint8_t>(pubkey, pubkey + size);
        }

        std::vector<uint8_t> ret(pubkey, pubkey + 33);
        ret[0] |= (pubkey[64] & 0x01);
        ret[0] -= 2;

        return ret;
    }

    std::vector<uint8_t> PubKey::Decompress(const uint8_t *pubkey, size_t size) {
        
        if (size != 33) {
            BKERROR() << "Invalid pubkey size:" << size;
            return std::vector<uint8_t>(pubkey, pubkey + size);
        }

        std::vector<uint8_t> ret(pubkey, pubkey + 33);

        secp256k1_pubkey secppubkey;
        if (!secp256k1_ec_pubkey_parse(crypto::ctx_secp256k1, &secppubkey, &ret[0], ret.size())) {
            BKERROR() << "secp256k1_ec_pubkey_parse failure";
            return std::vector<uint8_t>(pubkey, pubkey + size);
        }

        ret.resize(65);
        size_t publen = 65;
        secp256k1_ec_pubkey_serialize(crypto::ctx_secp256k1, &ret[0], &publen, &secppubkey, SECP256K1_EC_UNCOMPRESSED);

        ret.resize(publen);

        return ret;
    }
}
