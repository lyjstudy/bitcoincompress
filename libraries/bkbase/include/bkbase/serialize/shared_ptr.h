#pragma once

#include <memory>

namespace bkbase
{

/**
 * shared_ptr
 */
template <typename Stream, typename T>
void Serialize(Stream &os, const std::shared_ptr<const T> &p) {
    ::bkbase::Serialize(os, *p);
}

template <typename Stream, typename T>
void Unserialize(Stream &is, std::shared_ptr<const T> &p) {
    p = std::make_shared<const T>(deserialize, is);
}

} // bkbase
