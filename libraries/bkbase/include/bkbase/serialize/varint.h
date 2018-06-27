#pragma once

namespace bkbase
{

template<typename T, typename = std::enable_if<std::is_unsigned<T>::value>>
class VarInt {
protected:
    T &mNumber;
public:
    VarInt(T &num) : mNumber(num) {}

    template<typename Stream>
    void Serialize(Stream &stream) const {
        uint64_t n = mNumber;
        uint8_t tmp[(sizeof(n) * 8 + 6) / 7];
        int len = 0;
        while (true) {
            tmp[len] = (n & 0x7F) | (len ? 0x80 : 0x00);
            if (n <= 0x7F) {
                break;
            }
            n = (n >> 7) - 1;
            len++;
        }
        do {
            stream.Write((const char *)&tmp[len], sizeof(uint8_t));
        } while (len--);
    }
    template<typename Stream>
    void Unserialize(Stream &stream) {
        uint64_t n = 0;
        while (true) {
            uint8_t chData;
            if (stream.Read((char *)&chData, 1) != 1) {
                SerializeThrow("Unserialize VarInt failure");
            }
            n = (n << 7) | (chData & 0x7F);
            if ((chData & 0x80) == 0) {
                mNumber = n;
                return;
            }
            n++;
        }
        SerializeThrow("Unserialize VarInt failure");
    }
};

template<typename Stream, typename T>
inline void WriteVarInt(Stream &ss, T value) {
    ::bkbase::Serialize(ss, ::bkbase::VarInt<T>(value));
}

template<typename Stream, typename T>
inline void ReadVarInt(Stream &ss, T &value) {
    ::bkbase::VarInt<T> varint(value);
    ::bkbase::Unserialize(ss, varint);
}

} // bkbase
