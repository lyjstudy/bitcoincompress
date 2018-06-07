#pragma once

#include <stdint.h>
#include <vector>
#include <stdlib.h>

/**
 * A canonical signature exists of: <30> <total len> <02> <len R> <R> <02> <len
 * S> <S> <hashtype>, where R and S are not negative (their first byte has its
 * highest bit not set), and not excessively padded (do not start with a 0 byte,
 * unless an otherwise negative number follows, in which case a single 0 byte is
 * necessary and even required).
 *
 * See https://bitcointalk.org/index.php?topic=8392.msg127623#msg127623
 *
 * This function is consensus-critical since BIP66.
 */
bool IsValidSignatureEncoding(const uint8_t *sig, size_t size);

inline bool IsValidSignatureEncoding(const std::vector<uint8_t> &sig) {
    if (sig.empty()) return false;
    return IsValidSignatureEncoding(&sig[0], sig.size());
}