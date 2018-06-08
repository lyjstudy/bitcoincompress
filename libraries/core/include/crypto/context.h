#pragma once

#include <secp256k1.h>

namespace crypto {

    class Secp256k1Context {
    private:
        secp256k1_context *mCtx;
    public:
        Secp256k1Context() : mCtx(nullptr) {}
        ~Secp256k1Context() {
            Free();
        }
        operator secp256k1_context*() {
            if (mCtx == nullptr) {
                mCtx = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
            }
            return mCtx;
        }
        void Free() {
            if (mCtx != nullptr)
                secp256k1_context_destroy(mCtx);
            mCtx = nullptr;
        }
    };
    
    Secp256k1Context thread_local ctx_secp256k1;
}