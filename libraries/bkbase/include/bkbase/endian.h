#pragma once

#include <boost/endian/conversion.hpp>


namespace bkbase {

namespace endian_detail {

template<typename T> struct Access {
    using StoreType = T;
};
template<> struct Access<float> {
    using StoreType = uint32_t;
};
template<> struct Access<double> {
    using StoreType = uint64_t;
};
template<> struct Access<bool> {
    using StoreType = uint8_t;
};

} // endian_detail

template<typename T> static inline T ReadLE(const void *buffer) BOOST_NOEXCEPT {
    using StoreType = typename endian_detail::Access<T>::StoreType;
    BOOST_STATIC_ASSERT(sizeof(StoreType) == sizeof(T));
    StoreType value = boost::endian::little_to_native(*(StoreType *)buffer);
    return *(T *)&value;
}

template<typename T> static inline void WriteLE(const void *buffer, T data) BOOST_NOEXCEPT {
    using StoreType = typename endian_detail::Access<T>::StoreType;
    BOOST_STATIC_ASSERT(sizeof(StoreType) == sizeof(T));
    *(StoreType *)buffer = boost::endian::native_to_little(*(StoreType *)&data);
}

template<typename T> static inline T ReadBE(const void *buffer) BOOST_NOEXCEPT {
    using StoreType = typename endian_detail::Access<T>::StoreType;
    BOOST_STATIC_ASSERT(sizeof(StoreType) == sizeof(T));
    StoreType value = boost::endian::big_to_native(*(StoreType *)buffer);
    return *(T *)&value;
}

template<typename T> static inline void WriteBE(const void *buffer, T data) BOOST_NOEXCEPT {
    using StoreType = typename endian_detail::Access<T>::StoreType;
    BOOST_STATIC_ASSERT(sizeof(StoreType) == sizeof(T));
    *(StoreType *)buffer = boost::endian::native_to_big(*(StoreType *)&data);
}

} // bkbase