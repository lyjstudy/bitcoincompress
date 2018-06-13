#pragma once

#include <stdint.h>

namespace bkbase
{

// Numeric Serialize
#define SERIALIZER_DECLARE_NUMBER_TYPE(type) \
template<typename Stream> \
inline void Serialize(Stream &s, const type &value) { \
    type leValue; \
    WriteLE<type>(&leValue, value); \
    s.Write((const char *)&leValue, sizeof(type)); \
} \
template<typename Stream> \
inline void Unserialize(Stream &s, type &obj) { \
    type leValue; \
    if (s.Read((char *)&leValue, sizeof(type)) != sizeof(type)) SerializeThrow("Unserialize " #type " failure"); \
    obj = ReadLE<type>(&leValue); \
}

#define SERIALIZER_INTEGER_FOR_EACH(MACRO) \
    MACRO(uint8_t) \
    MACRO(uint16_t) \
    MACRO(uint32_t) \
    MACRO(uint64_t) \
    MACRO(int8_t) \
    MACRO(int16_t) \
    MACRO(int32_t) \
    MACRO(int64_t) \
    MACRO(char) \
    MACRO(bool) \
    MACRO(float) \
    MACRO(double) \

// Numeric Serialize
SERIALIZER_INTEGER_FOR_EACH(SERIALIZER_DECLARE_NUMBER_TYPE)

} // bkbase
