#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <vector>
#include <limits>
#include <memory>

namespace core {

    inline bool SwapBytes(const bool value) { return value; }
    inline char SwapBytes(const char value) { return value; }
    inline int8_t SwapBytes(const int8_t value) { return value; }
    inline uint8_t SwapBytes(const uint8_t value) { return value; }

    // TODO: swap bytes uint16 int16 uint32 int32 uint64 int64 float double
    inline uint16_t SwapBytes(const uint16_t value) { return value; }
    inline int16_t SwapBytes(const int16_t value) { return value; }
    inline uint32_t SwapBytes(const uint32_t value) { return value; }
    inline int32_t SwapBytes(const int32_t value) { return value; }
    inline uint64_t SwapBytes(const uint64_t value) { return value; }
    inline int64_t SwapBytes(const int64_t value) { return value; }
    inline float SwapBytes(const float value) { return value; }
    inline double SwapBytes(const double value) { return value; }

    template<typename T, typename T2 = uint8_t>
    inline T MemAs(const T2 *buf) {
        return SwapBytes(*(T *)buf);
    }

    constexpr uint64_t COMPACT_SIZE_MAX = 0x02000000;

    /*
        Serialize:
            Stream stream;
            Serialize(stream, obj)
            Unserialize(stream, obj)

        Stream:
            int GetType()
            int GetVersion()
            size_t Size()
        WritableStream:
            void Reserve(size_t size)
            void Write(const char *buf, size_t size)
            void Clear();
        ReadableStream:
            void Rewind()
            void Skip(size_t)
            size_t Read(char *buf, size_t size)
            size_t Pos()
    */

    class BaseStream {
    protected:
        const int mType;
        const int mVersion;
    public:
        explicit BaseStream(int type, int version)
            : mType(type), mVersion(version) {}
        inline int GetType() const { return mType; }
        inline int GetVersion() const { return mVersion; }
    };

    // WritableStream
    class SizeOfStream : public BaseStream {
    protected:
        size_t mSize;
    public:
        explicit SizeOfStream(int type, int version)
            : BaseStream(type, version), mSize(0) {}

        inline void Write(const char *, size_t size) {
            mSize += size;
        }
        inline size_t Size() const { return mSize; }
        inline void Reserve(size_t) {}
        inline void Clear() { mSize = 0; }
    };

    // ReadableStream
    class BufferReadStream : public BaseStream {
    protected:
        const char *mBuffer;
        size_t mSize;
        size_t mPos;
    public:
        explicit BufferReadStream(int type, int version, const char *buffer, size_t size)
            : BaseStream(type, version), mBuffer(buffer), mSize(size), mPos(0) {}

        inline void Rewind() { mPos = 0; }
        inline size_t Size() const { return mSize; }
        inline size_t Pos() const { return mPos; }
        inline bool Eof() const { return Pos() >= Size(); }
        void Skip(size_t size) {
            mPos += size;
            if (mPos > Size()) {
                mPos = Size();
            }
        }
        size_t Read(char *buf, size_t size) {
            size_t start = mPos;
            Skip(size);
            size_t end = mPos;
            if (start >= Size() || end == start) {
                return 0;
            }
            size_t retSize = end - start;
            memcpy(buf, &mBuffer[start], retSize);
            return retSize;
        }
    };

    // WritableStream && ReadableStream
    class VectorStream : public BaseStream {
    protected:
        std::vector<uint8_t> &mBuffer;
        size_t mPos;
    public:
        explicit VectorStream(int type, int version, std::vector<uint8_t> &buffer)
            : BaseStream(type, version), mBuffer(buffer), mPos(0) {}

        inline size_t Size() const { return mBuffer.size(); }
        
        inline void Clear() { mBuffer.clear(); }
        // WritableStream
        void Reserve(size_t size) { mBuffer.reserve(mBuffer.size() + size); }
        void Write(const char *buf, size_t size) {
            size_t start = mBuffer.size();
            mBuffer.resize(start + size);
            memcpy(&mBuffer[start], buf, size);
        }
        // ReadableStream
        inline void Rewind() { mPos = 0; }
        inline size_t Pos() const { return mPos; }
        inline bool Eof() const { return Pos() >= Size(); }
        void Skip(size_t size) {
            mPos += size;
            if (mPos > Size()) {
                mPos = Size();
            }
        }
        size_t Read(char *buf, size_t size) {
            size_t start = mPos;
            Skip(size);
            size_t end = mPos;
            if (start >= Size() || end == start) {
                return 0;
            }
            size_t retSize = end - start;
            memcpy(buf, &mBuffer[start], retSize);
            return retSize;
        }
    };

    class DataStream : public VectorStream {
    protected:
        std::vector<uint8_t> mSelfBuffer;
    public:
        explicit DataStream(int type, int version)
            : VectorStream(type, version, mSelfBuffer) {}
        
        std::vector<uint8_t> &Buffer() {
            return mSelfBuffer;
        }
        const std::vector<uint8_t> &Buffer() const {
            return mSelfBuffer;
        }
    };

    // Generic Serialize
    template<typename Stream, typename Object>
    inline void Serialize(Stream &s, const Object &obj) {
        obj.Serialize(s);
    }
    template<typename Stream, typename Object>
    inline bool Unserialize(Stream &s, Object &obj) {
        return obj.Unserialize(s);
    }

    #define SERIALIZER_DECLARE_NUMBER_TYPE(type) \
    template<typename Stream> \
    inline void Serialize(Stream &s, const type &obj) { \
        auto swapVal = SwapBytes(obj); \
        s.Write((const char *)&swapVal, sizeof(type)); \
    } \
    template<typename Stream> \
    inline bool Unserialize(Stream &s, type &obj) { \
        if (s.Read((char *)&obj, sizeof(type)) != sizeof(type)) return false; \
        obj = SwapBytes(obj); \
        return true; \
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

    // basic number serialize
    SERIALIZER_INTEGER_FOR_EACH(SERIALIZER_DECLARE_NUMBER_TYPE)
    SERIALIZER_DECLARE_NUMBER_TYPE(float)
    SERIALIZER_DECLARE_NUMBER_TYPE(double)

    template<typename T>
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
        bool Unserialize(Stream &stream) {
            uint64_t n = 0;
            while (true) {
                uint8_t chData;
                if (stream.Read((char *)&chData, 1) != 1) {
                    return false;
                }
                n = (n << 7) | (chData & 0x7F);
                if ((chData & 0x80) == 0) {
                    mNumber = n;
                    return true;
                }
                n++;
            }
        }
    };

    template<typename T>
    class CompactSize {
    protected:
        T &mNumber;
    public:
        explicit CompactSize(T &num)
            : mNumber(num) {}

        template<typename Stream>
        void Serialize(Stream &stream) const {
            if (mNumber < 253) {
                ::core::Serialize(stream, (uint8_t)mNumber);
            } else if (mNumber <= std::numeric_limits<uint16_t>::max()) {
                ::core::Serialize(stream, (uint8_t)253);
                ::core::Serialize(stream, (uint16_t)mNumber);
            } else if (mNumber <= std::numeric_limits<uint32_t>::max()) {
                ::core::Serialize(stream, (uint8_t)254);
                ::core::Serialize(stream, (uint32_t)mNumber);
            } else {
                ::core::Serialize(stream, (uint8_t)255);
                ::core::Serialize(stream, (uint64_t)mNumber);
            }
            return;
        }
        template<typename Stream>
        bool Unserialize(Stream &stream) {
            uint8_t chSize;
            if (stream.Read((char *)&chSize, 1) != 1) {
                return false;
            }
            bool ret;
            if (chSize < 253) {
                ret = true;
                mNumber = chSize;
            } else if (chSize == 253) {
                uint16_t nSizeRet;
                ret = ::core::Unserialize(stream, nSizeRet);
                if (ret && nSizeRet >= 253) {
                    mNumber = nSizeRet;
                } else {
                    ret = false;
                }
            } else if (chSize == 254) {
                uint32_t nSizeRet;
                ret = ::core::Unserialize(stream, nSizeRet);
                if (ret && nSizeRet >= 0x10000u && nSizeRet <= COMPACT_SIZE_MAX) {
                    mNumber = nSizeRet;
                } else {
                    ret = false;
                }
            } else {
                uint64_t nSizeRet;
                ret = ::core::Unserialize(stream, nSizeRet);
                if (ret && nSizeRet >= 0x100000000ULL && nSizeRet <= COMPACT_SIZE_MAX) {
                    mNumber = nSizeRet;
                } else {
                    ret = false;
                }
            }
            return ret;
        }
    };

    /*
        Wrap:
            Construct(type)
            Serialize()
            Unserialize()
    */
    #define SERIALIZER_DECLARE_WRAP_TYPE(prefix, type, wraptype) \
    template<typename Stream> \
    inline void Serialize##prefix(Stream &s, const wraptype &value) { \
        type<const wraptype> wrapObject(value); \
        wrapObject.Serialize<Stream>(s); \
    } \
    template<typename Stream> \
    inline bool Unserialize##prefix(Stream &s, wraptype &value) { \
        type<wraptype> wrapObject(value); \
        return wrapObject.Unserialize<Stream>(s); \
    }

    #define SERIALIZER_DECLARE_WRAP_VARINT(type) SERIALIZER_DECLARE_WRAP_TYPE(VarInt, core::VarInt, type)
    #define SERIALIZER_DECLARE_WRAP_COMPACT(type) SERIALIZER_DECLARE_WRAP_TYPE(CompactSize, core::CompactSize, type)

    SERIALIZER_INTEGER_FOR_EACH(SERIALIZER_DECLARE_WRAP_VARINT)
    SERIALIZER_INTEGER_FOR_EACH(SERIALIZER_DECLARE_WRAP_COMPACT)

    template<typename T>
    inline size_t GetSerializeSize(const T &obj, int type, int version = 0) {
        SizeOfStream stream(type, version);
        ::core::Serialize(stream, obj);
        return stream.Size();
    }

    // shared_ptr
    template <typename Stream, typename T>
    void Serialize(Stream &os, const std::shared_ptr<T> &p) {
        Serialize(os, *p);
    }

    template <typename Stream, typename T>
    bool Unserialize(Stream &is, std::shared_ptr<T> &p) {
        // TODO: Add Deserializer Constructor
        T obj;
        if (!Unserialize(is, obj)) return false;
        auto out = std::make_shared<T>(obj);
        p = out;
        return true;
    }
    // TODO: string limitstring

    // vector
    template <typename Stream, typename T, typename A>
    void Serialize_impl(Stream &os, const std::vector<T, A> &v, const uint8_t &) {
        ::core::SerializeCompactSize(os, v.size());
        if (!v.empty()) {
            os.Write((char *)&v[0], v.size() * sizeof(T));
        }
    }
    template <typename Stream, typename T, typename A, typename V>
    void Serialize_impl(Stream &os, const std::vector<T, A> &v, const V &) {
        ::core::SerializeCompactSize(os, v.size());
        for (const T &i : v) {
            ::core::Serialize(os, i);
        }
    }

    template <typename Stream, typename T, typename A>
    inline void Serialize(Stream &os, const std::vector<T, A> &v) {
        Serialize_impl(os, v, T());
    }

    template <typename Stream, typename T, typename A>
    bool Unserialize_impl(Stream &is, std::vector<T, A> &v, const uint8_t &) {
        v.clear();
        size_t nSize;
        if (!::core::UnserializeCompactSize(is, nSize)) return false;
        v.resize(nSize);
        if (is.Read((char *)&v[0], nSize * sizeof(T)) != nSize) return false;
        return true;
    }

    template <typename Stream, typename T, typename A, typename V>
    bool Unserialize_impl(Stream &is, std::vector<T, A> &v, const V &) {
        v.clear();
        size_t nSize;
        if (!::core::UnserializeCompactSize(is, nSize)) return false;
        v.resize(nSize);
        for (size_t i = 0; i < nSize; i++) {
            if (!::core::Unserialize(is, v[i])) return false;
        }
        return true;
    }

    template <typename Stream, typename T, typename A>
    inline bool Unserialize(Stream &is, std::vector<T, A> &v) {
        return Unserialize_impl(is, v, T());
    }

    // class
    enum {
        SER_NETWORK = (1 << 0),
        SER_DISK = (1 << 1),
        SER_GETHASH = (1 << 2),
    };

    struct SerActionSerialize {
        constexpr bool ForRead() const { return false; }
    };
    struct SerActionUnserialize {
        constexpr bool ForRead() const { return true; }
    };

    template <typename Stream, typename T>
    inline bool SerReadWrite(Stream &s, const T &obj, SerActionSerialize serAction) {
        ::core::Serialize(s, obj);
        return true;
    }
    template <typename Stream, typename T>
    inline bool SerReadWrite(Stream &s, T &obj, SerActionUnserialize serAction) {
        return ::core::Unserialize(s, obj);
    }

    #define READWRITE(obj) if (!(::core::SerReadWrite(s, (obj), serAction))) { return false; }
    #define READWRITEVARINT(obj) { \
        core::VarInt<decltype(obj)> wrap(obj); \
        if (!(::core::SerReadWrite(s, wrap, serAction))) { return false; } \
    }
    #define READWRITECOMPACT(obj) { \
        core::CompactSize<decltype(obj)> wrap(obj); \
        if (!(::core::SerReadWrite(s, wrap, serAction))) { return false; } \
    }

    template <typename T> inline T *NCONST_PTR(const T *val) {
        return const_cast<T *>(val);
    }

    #define ADD_SERIALIZE_METHODS \
        public: \
        template <typename Stream> void Serialize(Stream &s) const { \
            core::NCONST_PTR(this)->SerializationOp(s, core::SerActionSerialize()); \
        } \
        template <typename Stream> bool Unserialize(Stream &s) { \
            return SerializationOp(s, core::SerActionUnserialize()); \
        }
}
