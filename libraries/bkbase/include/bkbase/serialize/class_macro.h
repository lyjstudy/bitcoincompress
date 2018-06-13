#pragma once

namespace bkbase
{

struct SerActionSerialize {
    constexpr bool ForRead() const { return false; }
};
struct SerActionUnserialize {
    constexpr bool ForRead() const { return true; }
};

template <typename Stream, typename T>
inline void SerReadWrite(Stream &s, const T &obj, SerActionSerialize serAction) {
    ::bkbase::Serialize(s, obj);
}
template <typename Stream, typename T>
inline void SerReadWrite(Stream &s, T &obj, SerActionUnserialize serAction) {
    ::bkbase::Unserialize(s, obj);
}

template <typename T> inline T *NCONST_PTR(const T *val) {
    return const_cast<T *>(val);
}

#define ADD_SERIALIZE_METHODS \
    public: \
    template <typename Stream> void Serialize(Stream &s) const { \
        ::bkbase::NCONST_PTR(this)->SerializationOp(s, ::bkbase::SerActionSerialize()); \
    } \
    template <typename Stream> void Unserialize(Stream &s) { \
        SerializationOp(s, ::bkbase::SerActionUnserialize()); \
    }

#define READWRITE(obj) ::bkbase::SerReadWrite(s, (obj), serAction)

} // bkbase
