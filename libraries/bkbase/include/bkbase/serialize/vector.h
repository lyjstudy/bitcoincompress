#pragma once

namespace bkbase {


/**
 * vector
 */
template <typename Stream, typename T, typename A>
void Serialize_impl(Stream &os, const std::vector<T, A> &v, const uint8_t &) {
    WriteCompactSize(os, v.size());
    if (!v.empty()) {
        os.Write((char *)&v[0], v.size() * sizeof(T));
    }
}

template <typename Stream, typename T, typename A, typename V>
void Serialize_impl(Stream &os, const std::vector<T, A> &v, const V &) {
    WriteCompactSize(os, v.size());
    for (const T &i : v) {
        ::bkbase::Serialize(os, i);
    }
}

template <typename Stream, typename T, typename A>
inline void Serialize(Stream &os, const std::vector<T, A> &v) {
    Serialize_impl(os, v, T());
}

template <typename Stream, typename T, typename A>
void Unserialize_impl(Stream &is, std::vector<T, A> &v, const uint8_t &) {
    // Limit size per read so bogus size value won't cause out of memory
    v.clear();
    size_t nSize = ReadCompactSize(is);
    size_t i = 0;
    while (i < nSize) {
        size_t blk = std::min(nSize - i, size_t(1 + 4999999 / sizeof(T)));
        v.resize(i + blk);
        is.Read((char *)&v[i], blk * sizeof(T));
        i += blk;
    }
}

template <typename Stream, typename T, typename A, typename V>
void Unserialize_impl(Stream &is, std::vector<T, A> &v, const V &) {
    v.clear();
    size_t nSize = ReadCompactSize(is);
    size_t i = 0;
    size_t nMid = 0;
    while (nMid < nSize) {
        nMid += 5000000 / sizeof(T);
        if (nMid > nSize) {
            nMid = nSize;
        }
        v.resize(nMid);
        for (; i < nMid; i++) {
            Unserialize(is, v[i]);
        }
    }
}

template <typename Stream, typename T, typename A>
inline void Unserialize(Stream &is, std::vector<T, A> &v) {
    Unserialize_impl(is, v, T());
}

}