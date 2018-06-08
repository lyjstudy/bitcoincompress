#include <crypto/context.h>
#include <compress/pubkey.h>

namespace compress {

    std::vector<uint8_t> PubKey::Compress(const uint8_t *pubkey, size_t size) {
        
        if (size != 65 || pubkey[0] != 0x04)
            return std::vector<uint8_t>(pubkey, pubkey + size);

        std::vector<uint8_t> ret(pubkey, pubkey + 33);
        ret[0] |= (pubkey[64] & 0x01);

        return ret;
    }

    std::vector<uint8_t> PubKey::Decompress(const uint8_t *pubkey, size_t size) {
        
        if (size != 33 || (pubkey[0] != 0x04 && pubkey[0] != 0x05))
            return std::vector<uint8_t>(pubkey, pubkey + size);

        std::vector<uint8_t> ret(pubkey, pubkey + 33);
        ret[0] -= 2;

        secp256k1_pubkey secppubkey;
        if (!secp256k1_ec_pubkey_parse(crypto::ctx_secp256k1, &secppubkey, &ret[0], ret.size())) {
            return std::vector<uint8_t>(pubkey, pubkey + size);
        }

        ret.resize(65);
        size_t publen = 65;
        secp256k1_ec_pubkey_serialize(crypto::ctx_secp256k1, &ret[0], &publen, &secppubkey, SECP256K1_EC_UNCOMPRESSED);

        ret.resize(publen);

        return ret;
    }
}
