#include "hash.h"
#include <openssl/evp.h>

namespace core {

    class EvpContext {
    protected:
        EVP_MD_CTX *mCtx;
    public:
        EvpContext()
            : mCtx(nullptr) {}
        ~EvpContext() {
            if (mCtx != nullptr) {
                EVP_MD_CTX_free(mCtx);
            }
        }
        EVP_MD_CTX *Ctx() {
            if (mCtx == nullptr) {
                mCtx = EVP_MD_CTX_new();
            }
            return mCtx;
        }
    };

    static thread_local EvpContext GEVPContext;

    using GetMDType = decltype(EVP_sha256);

    // TODO: add log
    template<GetMDType HashType>
    bool CalcHash(const char *mem, size_t size, unsigned char *out) {
        if (GEVPContext.Ctx() != nullptr) {
            // EVP_DigestInit_ex(), EVP_DigestUpdate() and EVP_DigestFinal_ex() return 1 for success and 0 for failure.
            if (EVP_DigestInit_ex(GEVPContext.Ctx(), HashType(), nullptr) != 1) return false;
            if (EVP_DigestUpdate(GEVPContext.Ctx(), mem, size) != 1) return false;
            if (EVP_DigestFinal_ex(GEVPContext.Ctx(), out, nullptr) != 1) return false;
            return true;
        }
        return false;
    }
    
    Uint256 HashSha256(const char *mem, size_t size) {
        Uint256 hash;
        if (!CalcHash<EVP_sha256>(mem, size, hash.begin())) {
            hash.SetZero();
        }
        return hash;
    }

    Uint256 HashDoubleSha256(const char *mem, size_t size) {
        Uint256 hash;
        if (!CalcHash<EVP_sha256>(mem, size, hash.begin())) {
            hash.SetZero();
        } else if (!CalcHash<EVP_sha256>((const char *)hash.begin(), hash.size(), hash.begin())) {
            hash.SetZero();
        }
        return hash;
    }
    
    Uint160 Ripemd160(const char *mem, size_t size) {
        Uint160 hash;
        if (!CalcHash<EVP_ripemd160>(mem, size, hash.begin())) {
            hash.SetZero();
        }
        return hash;
    }
}
