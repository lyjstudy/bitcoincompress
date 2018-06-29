#pragma once

#include <bkbase/endian.h>
#include <bkbase/exception.h>

namespace bkbase {


struct deserialize_type {};
constexpr deserialize_type deserialize{};

// Exception
struct SerializeException : virtual Exception {};
BK_DEFINE_ERROR_INFO(SerializeError, const char *);
#define SerializeThrow(msg) BK_THROW(SerializeException() << SerializeError(msg))

// Object Serialize
template<typename Stream, typename Object>
void Serialize(Stream &s, const Object &obj) {
    obj.Serialize(s);
}
template<typename Stream, typename Object>
void Unserialize(Stream &s, Object &obj) {
    obj.Unserialize(s);
}

} // bkbase

