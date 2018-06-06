#include "hex.h"

namespace core {
    const signed char HEX_TO_DIGIT[256] = {
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
         0,  1,   2,   3,   4,   5,   6,   7,  8,  9, -1, -1, -1, -1, -1, -1,
        -1, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    };

    signed char HexDigit(char c) {
        return HEX_TO_DIGIT[(uint8_t)c];
    }
    char DigitHex(char value) {
        switch (value) {
            case 0:
                return '0';
            case 1:
                return '1';
            case 2:
                return '2';
            case 3:
                return '3';
            case 4:
                return '4';
            case 5:
                return '5';
            case 6:
                return '6';
            case 7:
                return '7';
            case 8:
                return '8';
            case 9:
                return '9';
            case 10:
                return 'a';
            case 11:
                return 'b';
            case 12:
                return 'c';
            case 13:
                return 'd';
            case 14:
                return 'e';
            case 15:
                return 'f';
            default:
                break;
        }
        return 0;
    }

    size_t HexToBin(const char *psz, uint8_t *buf, size_t size) {
        size_t current = 0;
        while (true) {
            while (isspace(*psz))
                psz++;
            if (current >= size) break;
            signed char c = HexDigit(*psz++);
            if (c == (signed char)-1) break;
            uint8_t n = (c << 4);
            c = HexDigit(*psz++);
            if (c == (signed char)-1) break;
            n |= c;
            buf[current++] = n;
        }
        return current;
    }

    std::string HexFromBin(const uint8_t *buf, size_t size) {
        std::string str;
        str.reserve(size * 2);
        for (size_t i = 0; i < size; i++) {
            str.push_back(DigitHex(buf[i] >> 4));
            str.push_back(DigitHex(buf[i] & 0xF));
        }
        return std::move(str);
    }

    size_t HexToReverseBin(const char *psz, uint8_t *buf, size_t size) {
        // skip leading spaces
        while (isspace(*psz))
            psz++;

        // skip 0x
        if (psz[0] == '0' && tolower(psz[1]) == 'x') psz += 2;

        // hex string to uint
        const char *pbegin = psz;
        while (HexDigit(*psz) != -1)
            psz++;
        psz--;
        uint8_t *p1 = (uint8_t *)buf;
        uint8_t *pend = p1 + size;
        while (psz >= pbegin && p1 < pend) {
            *p1 = HexDigit(*psz--);
            if (psz >= pbegin) {
                *p1 |= uint8_t(HexDigit(*psz--) << 4);
            }
            p1++;
        }
        return p1 - buf;
    }

    std::string HexFromReverseBin(const uint8_t *buf, size_t size) {
        std::string str;
        str.reserve(size * 2);
        for (size_t i = 0; i < size; i++) {
            uint8_t b = buf[size - i - 1];
            str.push_back(DigitHex(b >> 4));
            str.push_back(DigitHex(b & 0xF));
        }
        return std::move(str);
    }
}