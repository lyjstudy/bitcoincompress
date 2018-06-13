#include <bkbase/stream.h>

namespace bkbase {

///////////////////////////StreamReadPtr///////////////////////////
void StreamReadPtr::Skip(size_t size) {
    mPos += size;
    if (mPos > Size()) {
        mPos = Size();
    }
}
size_t StreamReadPtr::Read(char *buf, size_t size) {
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

///////////////////////////StreamVector///////////////////////////
void StreamVector::Write(const char *buf, size_t size) {
    size_t start = mBuffer.size();
    mBuffer.resize(start + size);
    memcpy(&mBuffer[start], buf, size);
}
void StreamVector::Skip(size_t size) {
    mPos += size;
    if (mPos > Size()) {
        mPos = Size();
    }
}
size_t StreamVector::Read(char *buf, size_t size) {
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

} // bkbase 