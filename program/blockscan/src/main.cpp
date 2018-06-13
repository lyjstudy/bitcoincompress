#include "main.h"

/*

    脚本压缩：
        1. NonStandard 解锁和锁定脚本都直接保存原始数据
        2. OpReturn 锁定脚本保存原始数据
        3. PubKey 锁定脚本: 可以提取公钥的压缩格式保存
                  标准解锁脚本: PUSH(signature)
                        Format: 0x30 [total-length] 0x02 [R-length] [R] 0x02 [S-length] [S] [sighash]
                            => [sighash] [R-length] [R] [S] sighash使用0167 R-len使用345 [29, 34]
        4. PubKeyHash 锁定脚本: 指向pubkey 如果是未花费输出很有可能没有pubkey此时直接存hash
                      标准解锁脚本: PUSH(signature) PUSH(pubkey)
                                signature压缩 pubkey压缩
        5. MultiSig 锁定脚本: pubkey列表
                    标准解锁脚本 类似PubKey的方案
        6. ScriptHash 
*/


class OutputPool {
protected:
    OutputMap mPool;
    std::unordered_map<core::Uint256, core::Block *, core::HashOperator<core::Uint256>> mCachedBlock;
    core::Uint256 mChainTip;
    int mChainHeight;
public:
    inline void Status() {
        printf("Chain Height: %d, Cached: %zd\n", mChainHeight, mCachedBlock.size());
        mPool.Status();
    }
    OutputPool(OutputMap::LogFuncType func) : mPool(func), mChainHeight(0) {}

    void FlushBlk() {
        auto finded = mCachedBlock.find(mChainTip);
        if (finded == mCachedBlock.end()) return;
        mChainHeight++;
        auto blk = finded->second;
        mCachedBlock.erase(finded);
        mChainTip = blk->GetHash();
        ProcessBlk(blk);
        delete blk;
        FlushBlk();
    }

    void AddBlk(core::Block *blk) {
        if (blk->HashPrevBlock() == mChainTip) {
            mChainHeight++;
            mChainTip = blk->GetHash();
            ProcessBlk(blk);
            delete blk;
            FlushBlk();
        } else {
            if (mCachedBlock.find(blk->HashPrevBlock()) == mCachedBlock.end()) {
                mCachedBlock.insert(std::make_pair(blk->HashPrevBlock(), blk));
            } else {
                LOGE("FORK BLOCK %s => %s", blk->HashPrevBlock().GetHex().c_str(), blk->GetHash().GetHex().c_str());
            }
        }
    }

    void ProcessBlk(core::Block *blk) {
        for (size_t i = 0; i < blk->GetTxCount(); i++) {
            auto tx = blk->GetTx(i);
            for (size_t j = 0; j < tx->GetOutputSize(); j++) {
                auto out = tx->GetOutput(j);
                auto type = script::GetOutputType(out.GetScript());
                switch (type) {
                    // case script::Type::PubKey:
                    // case script::Type::PubKeyHash:
                    case script::Type::ScriptHash:
                    // case script::Type::MultiSig:
                    // case script::Type::NonStandard:
                        mPool.AddOutput(tx, (uint16_t)j, type, out.GetScript());
                        break;
                    default: 
                        // mPool.AddOpReturn(tx, (uint16_t)j, out.GetScript());
                        break;
                }
            }
            if (i == 0) continue; // coinbase
            for (size_t j = 0; j < tx->GetInputSize(); j++) {
                auto in = tx->GetInput(j);
                mPool.AddInput(in.TxId(), in.TxIndex(), in.GetScript());
            }
        }
    }
};

void logfunc(const char *txid, uint32_t index, script::Type type, const std::vector<uint8_t> &lock, const std::vector<uint8_t> *unlock) {
    if (type == script::Type::PubKey) {
        if (unlock != nullptr) {
        }
    } else if (type == script::Type::PubKeyHash) {
        if (unlock != nullptr) {
            
        }
    } else if (type == script::Type::ScriptHash) {
        if (unlock != nullptr) {
            std::vector<uint8_t> scriptop;
            std::vector<std::vector<uint8_t>> scriptChild;
            script::Parser::GetScriptTemplate(*unlock, scriptop, &scriptChild);
            if (!scriptChild.empty()) {
                //printf("%s\n", GetOpString(scriptChild.back()).c_str());
                scriptop.clear();
                script::Parser::GetScriptTemplate(scriptChild.back(), scriptop);
                if (std::find(scriptop.begin(), scriptop.end(), OP_CHECKSEQUENCEVERIFY) != scriptop.end()) {
                    printf("%s %d\n", txid, (int)index);
                    printf("%s\n\n", GetOpString(*unlock).c_str());
                    printf("%s\n\n\n\n", GetOpString(scriptChild.back()).c_str());
                }
            }
            // printf("%s\n", core::HexFromBin(*unlock).c_str());
        }
    }
}

int main(int argc, char *argv[]) {
    
    utils::BlockLoader loader("/home/liuyujun/.bitcoin/blocks/");

    OutputPool pool(logfunc);

    loader.SetFilePos(1000, 0);

    for (size_t i = 0; i < 600000; i++) {
        if (i % 50000 == 0) {
            LOGI("processing: %d", (int)i);
            pool.Status();
        }
        if (!loader.NextBlock([&]() {
            core::Block *blk = new core::Block();
            loader.ReadBlock(*blk);
            pool.ProcessBlk(blk);
            delete blk;
        })) {
            break;
        }
    }
    return 0;
}