#include <crypto/context.h>

namespace crypto {

Secp256k1Context thread_local Secp256k1Sign(SECP256K1_CONTEXT_SIGN);
Secp256k1Context thread_local Secp256k1(SECP256K1_CONTEXT_VERIFY);

}