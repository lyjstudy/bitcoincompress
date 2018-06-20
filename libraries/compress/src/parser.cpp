#include <compress/parser.h>
#include <script/parser.h>

namespace compress {

bool ParseScript(const std::vector<uint8_t> &script, std::vector<uint8_t> &temp, std::vector<std::vector<uint8_t>> &dataArea) {
    temp.clear();
    dataArea.clear();

    script::Parser parser(script);
    std::vector<uint8_t> data;
    uint8_t opcode;

    while (parser.Fetch(opcode, &data)) {
        if (opcode <= OP_16) {
            if (opcode <= OP_PUSHDATA4) {
                dataArea.push_back(std::move(data));
            } else if (opcode == OP_1NEGATE) {
                dataArea.push_back({ 0x81 });
            } else {
                dataArea.push_back({ (uint8_t)(opcode + 1 - OP_1) });
            }
            temp.push_back(OP_INVALIDOPCODE);
        } else {
            temp.push_back(opcode);
        }
    }

    if (opcode != OP_INVALIDOPCODE) {
        return false;
    }
    return true;
}

bool GenScript(std::vector<uint8_t> &script, const std::vector<uint8_t> &temp, const std::vector<std::vector<uint8_t>> &dataArea) {

}

}
