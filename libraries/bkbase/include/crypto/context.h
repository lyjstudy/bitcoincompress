#pragma once

#include <secp256k1.h>

namespace crypto {

    class Secp256k1Context {
    private:
        secp256k1_context *mCtx;
        int mFlags;
    public:
        Secp256k1Context(int flags) : mCtx(nullptr), mFlags(flags) {}
        ~Secp256k1Context() {
            Free();
        }
        operator secp256k1_context*() {
            if (mCtx == nullptr) {
                mCtx = secp256k1_context_create(mFlags);
            }
            return mCtx;
        }
        void Free() {
            if (mCtx != nullptr)
                secp256k1_context_destroy(mCtx);
            mCtx = nullptr;
        }
    };
    
    extern Secp256k1Context thread_local Secp256k1Sign;
    extern Secp256k1Context thread_local Secp256k1;
}