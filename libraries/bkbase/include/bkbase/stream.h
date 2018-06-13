#pragma once

#include <stdint.h>
#include <memory.h>
#include <vector>
#include "serialize.h"

namespace bkbase {

/*
    StreamBase:
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
        bool Eof()
*/

enum {
    SER_NETWORK = (1 << 0),
    SER_DISK = (1 << 1),
    SER_GETHASH = (1 << 2),
};
    
class StreamBase {
protected:
    const int mType;
    const int mVersion;
public:
    explicit StreamBase(int type, int version)
        : mType(type), mVersion(version) {}
    inline int GetType() const { return mType; }
    inline int GetVersion() const { return mVersion; }
};

class StreamSizeOf : public StreamBase {
protected:
    size_t mSize;
public:
    explicit StreamSizeOf(int type, int version)
        : StreamBase(type, version), mSize(0) {}

    inline void Write(const char *, size_t size) {
        mSize += size;
    }
    inline size_t Size() const { return mSize; }
    inline void Reserve(size_t) {}
    inline void Clear() { mSize = 0; }

    template <typename T> inline StreamSizeOf &operator<<(const T &obj) {
        ::bkbase::Serialize(*this, obj);
        return (*this);
    }
};

class StreamReadPtr : public StreamBase {
protected:
    const char *mBuffer;
    size_t mSize;
    size_t mPos;
public:
    explicit StreamReadPtr(int type, int version, const void *buffer, size_t size)
        : StreamBase(type, version), mBuffer((const char *)buffer), mSize(size), mPos(0) {}

    inline void Rewind() { mPos = 0; }
    inline size_t Size() const { return mSize; }
    inline size_t Pos() const { return mPos; }
    inline bool Eof() const { return Pos() >= Size(); }
    void Skip(size_t size);
    size_t Read(char *buf, size_t size);

    template <typename T> inline StreamReadPtr &operator>>(T &obj) {
        ::bkbase::Unserialize(*this, obj);
        return (*this);
    }
};

class StreamVector : public StreamBase {
protected:
    std::vector<uint8_t> &mBuffer;
    size_t mPos;
public:
    explicit StreamVector(int type, int version, std::vector<uint8_t> &buffer)
        : StreamBase(type, version), mBuffer(buffer), mPos(0) {}

    inline size_t Size() const { return mBuffer.size(); }
    
    inline void Clear() { mBuffer.clear(); }
    // WritableStream
    inline void Reserve(size_t size) { mBuffer.reserve(mBuffer.size() + size); }
    void Write(const char *buf, size_t size);
    // ReadableStream
    inline void Rewind() { mPos = 0; }
    inline size_t Pos() const { return mPos; }
    inline bool Eof() const { return Pos() >= Size(); }
    void Skip(size_t size);
    size_t Read(char *buf, size_t size);

    template <typename T> inline StreamVector &operator>>(T &obj) {
        ::bkbase::Unserialize(*this, obj);
        return (*this);
    }
    template <typename T> inline StreamVector &operator<<(const T &obj) {
        ::bkbase::Serialize(*this, obj);
        return (*this);
    }
};

class StreamData : public StreamVector {
protected:
    std::vector<uint8_t> mSelfBuffer;
public:
    explicit StreamData(int type, int version)
        : StreamVector(type, version, mSelfBuffer) {}
    
    inline std::vector<uint8_t> &Buffer() {
        return mSelfBuffer;
    }
    inline const std::vector<uint8_t> &Buffer() const {
        return mSelfBuffer;
    }
    template <typename T> inline StreamData &operator>>(T &obj) {
        ::bkbase::Unserialize(*this, obj);
        return (*this);
    }
    template <typename T> inline StreamData &operator<<(const T &obj) {
        ::bkbase::Serialize(*this, obj);
        return (*this);
    }
};

template<typename Hasher>
class StreamHash : public StreamBase {
protected:
    Hasher mCtx;
public:
    using ResultType = typename Hasher::ResultType;
    explicit StreamHash(int type, int version)
        : StreamBase(type, version) {}

    inline void Write(const char *buf, size_t size) {
        mCtx.Write(buf, size);
    }
    inline size_t Size() const { return Hasher::OutputSize; }
    inline void Reserve(size_t) {}
    inline void Clear() { mCtx.Reset(); }
    inline ResultType GetHash() { return mCtx.Finalize(); }

    template <typename T> inline StreamHash &operator<<(const T &obj) {
        ::bkbase::Serialize(*this, obj);
        return (*this);
    }
};


template<typename T>
inline size_t GetSerializeSize(const T &obj, int type, int version = 0) {
    StreamSizeOf stream(type, version);
    ::bkbase::Serialize(stream, obj);
    return stream.Size();
}

} // bkbase
