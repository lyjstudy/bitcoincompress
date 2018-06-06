#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace core {

    size_t HexToBin(const char *psz, uint8_t *buf, size_t size);
    std::string HexFromBin(const uint8_t *buf, size_t size);
    size_t HexToReverseBin(const char *psz, uint8_t *buf, size_t size);
    std::string HexFromReverseBin(const uint8_t *buf, size_t size);

    inline std::string HexFromBin(const std::vector<uint8_t> &data) {
        if (data.empty()) return "";
        return HexFromBin(&data[0], data.size());
    }
}
