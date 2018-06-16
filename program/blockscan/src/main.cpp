#include "output_pool.h"
#include "scan.h"
#include <atomic>


class PubkeyScan : public OutputPool {
public:
    virtual bool IsUnspendable(const std::vector<uint8_t> &script, int64_t amount) {
        if (amount <= 0) return true;
        if (script.empty()) return true;
        if (script[0] == OP_RETURN) return true;
        return false;
    }
    virtual void OnOutputUnspendable(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock) {
    }
    virtual void OnTransaction(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock, const std::vector<uint8_t> &unlock) {
        std::vector<std::vector<uint8_t>> address;
        auto type = script::GetOutputType(lock, &address);
        switch (type) {
            case script::Type::PubKey:
                // 地址是公钥
                break;
            case script::Type::PubKeyHash:
                // 解锁脚本的最后一个PUSH是公钥
                break;
            case script::Type::ScriptHash:
                // 解锁脚本的最后一个PUSH 解出来的类型 再继续分析
                break;
            case script::Type::MultiSig:
                // 所有地址都是公钥
                break;
            default: // NonStandard Spended
                // 
                break;
        }
    }
    // Main Thread
    virtual void OnInput(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &unlock) {
    }
    virtual void OnOutput(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock) {
    }
};


int main(int argc, char *argv[]) {
    bkbase::LoggingInitialize(LOGCRITICAL, LOGTRACE);
    bkbase::LoggingThreadName("main");
    BKLOGSCOPE("entry");

    Scan<PubkeyScan> scan("/home/liuyujun/.bitcoin/blocks");

    scan.Start();
    return 0;
}