#pragma once

#include <algorithm>
#include <boost/endian/conversion.hpp>

namespace bkbase {

class DigitFormats {
protected:
    const static uint8_t INVALID = 0xFF;
    uint8_t mChar2Byte[256];
    uint8_t mByte2Char[256];
public:
    DigitFormats(const std::string &chars, const std::string &alterChars = "") {
        memset(mChar2Byte, 0xFF, sizeof(mChar2Byte));
        memset(mByte2Char, 0xFF, sizeof(mByte2Char));
        BOOST_VERIFY(chars.length() < 128);
        if (!alterChars.empty()) {
            BOOST_VERIFY(alterChars.length() == chars.length());
        }
        for (uint8_t i = 0; i < (uint8_t)chars.length(); i++) {
            uint8_t c = (uint8_t)chars[i];
            mChar2Byte[c] = i;
            mByte2Char[i] = c;
            if (!alterChars.empty()) {
                c = (uint8_t)chars[i];
                mChar2Byte[c] = i;
                mByte2Char[i] = c;
            }
        }
    }

    inline bool operator ()(char value) {
        return IsValidChar(value);
    }

    template<typename T>
    inline bool IsValidChar(T value) {
        return mChar2Byte[(uint8_t)value] != INVALID;
    }

    inline bool IsValidString(const std::string &str) {
        return std::all_of(str.begin(), str.end(), *this);
    }
};

// 2^BITS Digit2: 2^1 Digit4: 2^2 ...
// < 8
template<size_t BITS>
class DigitFormatsFast : public DigitFormats {
protected:
    // 1=>2 2=>4 3=>8 4=>16 5=>32 6=>64 7=>128
    const static size_t BITS_MASK = ((1 << BITS) - 1);

public:
    using DigitFormats::DigitFormats;

    inline size_t EncodeSize(size_t inSize) {
        const auto totalBits = inSize << 3;
        if (totalBits % BITS != 0) {
            return (totalBits / BITS) + 1;
        }
        return totalBits / BITS;
    }
    inline size_t DecodeSize(size_t size) {
        const auto totalBits = size * BITS;
        if (totalBits % 8 != 0) {
            return (totalBits / 8) + 1;
        }
        return totalBits / 8;
    }

    template<typename T>
    inline std::string Encode(const T &value) {
#ifdef BOOST_BIG_ENDIAN
        return Encode((const uint8_t *)&value, sizeof(T));
#else
        T le = boost::endian::endian_reverse(value);
        return Encode((const uint8_t *)&le, sizeof(T));
#endif
    }

    inline std::string Encode(const std::vector<uint8_t> &buffer) {
        if (buffer.empty()) return std::string();
        return Encode(&buffer[0], buffer.size());
    }
    std::string Encode(const uint8_t *buffer, size_t size) {
        std::string r;
        uint16_t value;
        size_t bitCount = 0;
        for (size_t i = 0; i < size; i++) {
            uint8_t b = buffer[i];

            bitCount += 8;
            value <<= 8;
            value |= b;
            
            const auto loopCnt = bitCount / BITS;
            for (size_t j = 0; j < loopCnt; j++) {
                uint8_t val = value & BITS_MASK;
                value >>= BITS;
                bitCount -= BITS;
                r.push_back((char)mByte2Char[val]);
            }
        }
        if (bitCount != 0) {
            r.push_back((char)mByte2Char[(uint8_t)value]);
        }
        std::reverse(r.begin(), r.end());
        return std::move(r);
    }

    bool Decode(const std::string &value, uint8_t *buffer, size_t outSize) {
        return true;
    }
};

class DigitFormatsSlow : public DigitFormats {

};

extern DigitFormatsFast<1> Digit2;
extern DigitFormatsFast<3> Digit8;
extern DigitFormats Digit10;
extern DigitFormatsFast<4> Digit16;
extern DigitFormatsFast<5> Digit32;
extern DigitFormats Digit58;
extern DigitFormatsFast<6> Digit64;

}