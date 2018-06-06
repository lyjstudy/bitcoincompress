
#include "transaction.h"

namespace core {
    Transaction::Transaction()
        : mVersion(Transaction::CURRENT_VERSION), mLockTime(0) {
    }
}