#pragma once

#include <stdint.h>
#include <vector>

namespace compress {

class ScriptDB {

public:
    uint64_t AddScript(const std::vector<uint8_t> &lock, const std::vector<uint8_t> &unlock);
    uint64_t AddLockScript(const std::vector<uint8_t> &lock);

};

}
