#include <compress/signature.h>

namespace compress {

    bool Signature::IsSupport(const uint8_t *sig, size_t size) {
        if (size < 9 || size > 73) return false;
        // 0x30
        if (sig[0] != 0x30) return false;
        // [total-length]
        if (sig[1] != size - 3) return false;
        // 0x02
        if (sig[2] != 0x02) return false;
        // lenR
        auto lenR = sig[3];
        if (5 + (size_t)lenR >= size) return false;
        if (!IsSupportLenR(lenR)) return false;
        // lenS
        auto lenS = sig[5 + lenR];
        if ((size_t)lenR + (size_t)lenS + 7 != size) return false;
        // sighash
        auto sighash = sig[size - 1];
        if (!IsSupportSighash(sighash)) return false;

        return true;
    }

    inline static std::vector<uint8_t> ConcatZero(const uint8_t *sig, size_t size) {
        std::vector<uint8_t> uncompressData;
        uncompressData.push_back(0);
        if (size > 0)
            uncompressData.insert(uncompressData.end(), sig, sig + size);
        return uncompressData;
    }

    std::vector<uint8_t> Signature::Compress(const uint8_t *sig, size_t size) {
        if (!IsSupport(sig, size)) return ConcatZero(sig, size);

        auto lenR = sig[3];
        auto R = &sig[4];
        auto lenS = sig[5 + lenR];
        auto S = sig[6 + lenR];
        auto sighash = sig[size - 1];

        std::vector<uint8_t> ret;

        ret.push_back(sighash | COMPRESSED_TAG | ((lenR - LENR_MIN) << 3));
        ret.insert(ret.end(), R, R + lenR);
        ret.insert(ret.end(), S, S + lenS);

        return ret;
    }

    std::vector<uint8_t> Signature::Decompress(const uint8_t *data, size_t size) {
        // Bad Format
        if (size == 0) return std::vector<uint8_t>();

        if (!IsCompressed(data[0])) return std::vector<uint8_t>(data + 1, data + size - 1);
        
        uint8_t sighash = data[0] & (~SIGHASH_MASK);
        uint8_t lenR = (data[0] >> 3) & 0x07;

        // Bad Format
        if (!IsSupportLenR(lenR)) return std::vector<uint8_t>();
        if ((size_t)lenR + 1 < size) return std::vector<uint8_t>();

        uint8_t lenS = size - lenR - 1;
        auto R = &data[1];
        auto S = &data[1 + lenR];
        std::vector<uint8_t> ret;
        ret.push_back(0x30);
        ret.push_back(lenS + lenR + 4);
        ret.push_back(0x02);
        ret.push_back(lenR);
        ret.insert(ret.end(), R, R + lenR);
        ret.push_back(lenS);
        if (lenS > 0) ret.insert(ret.end(), S, S + lenS);
        ret.push_back(sighash);

        return ret;
    }

}
