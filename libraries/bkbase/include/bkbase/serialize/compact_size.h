#pragma once

namespace bkbase
{
    
constexpr uint64_t COMPACT_SIZE_MAX = 0x02000000;

template<typename T, typename = std::enable_if<std::is_unsigned<T>::value>>
class CompactSize {
protected:
    T &mNumber;
public:
    explicit CompactSize(T &num)
        : mNumber(num) {}

    template<typename Stream>
    void Serialize(Stream &stream) const {
        if (mNumber < 253) {
            ::bkbase::Serialize(stream, (uint8_t)mNumber);
        } else if (mNumber <= std::numeric_limits<uint16_t>::max()) {
            ::bkbase::Serialize(stream, (uint8_t)253);
            ::bkbase::Serialize(stream, (uint16_t)mNumber);
        } else if (mNumber <= std::numeric_limits<uint32_t>::max()) {
            ::bkbase::Serialize(stream, (uint8_t)254);
            ::bkbase::Serialize(stream, (uint32_t)mNumber);
        } else {
            ::bkbase::Serialize(stream, (uint8_t)255);
            ::bkbase::Serialize(stream, (uint64_t)mNumber);
        }
    }
    template<typename Stream>
    void Unserialize(Stream &stream) {
        uint8_t chSize;
        if (stream.Read((char *)&chSize, 1) != 1) {
            SerializeThrow("Unserialize CompactSize failure");
        }
        if (chSize < 253) {
            mNumber = chSize;
        } else if (chSize == 253) {
            uint16_t nSizeRet;
            ::bkbase::Unserialize(stream, nSizeRet);
            if (nSizeRet < 253) {
                SerializeThrow("Unserialize CompactSize(uint16_t) failure");
            }
            mNumber = nSizeRet;
        } else if (chSize == 254) {
            uint32_t nSizeRet;
            ::bkbase::Unserialize(stream, nSizeRet);
            if (nSizeRet < 0x10000u || nSizeRet > COMPACT_SIZE_MAX) {
                SerializeThrow("Unserialize CompactSize(uint32_t) failure");
            }
            mNumber = nSizeRet;
        } else {
            uint64_t nSizeRet;
            ::bkbase::Unserialize(stream, nSizeRet);
            if (nSizeRet < 0x100000000ULL || nSizeRet > COMPACT_SIZE_MAX) {
                SerializeThrow("Unserialize CompactSize(uint64_t) failure");
            }
            mNumber = nSizeRet;
        }
    }
};

template<typename Stream>
inline void WriteCompactSize(Stream &ss, size_t value) {
    ::bkbase::Serialize(ss, ::bkbase::CompactSize<size_t>(value));
}

template<typename Stream>
inline size_t ReadCompactSize(Stream &ss) {
    size_t value;
    ::bkbase::CompactSize<size_t> compact(value);
    ::bkbase::Unserialize(ss, compact);
    return value;
}

} // bkbase
