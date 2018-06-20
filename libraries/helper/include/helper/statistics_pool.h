#pragma once

#include <helper/scan.h>
#include <helper/output_pool.h>
#include <script/opcode.h>
#include <script/type.h>
#include <atomic>
#include <iomanip>

namespace helper {

#define STATISTICS_LIST(MACRO) \
    MACRO(Unspendable, "不可花费输出\t\t\t\t") \
    MACRO(BadInput, "未查找到的输入\t\t\t\t") \
    \
    MACRO(UTXOP2PK, "未花费公钥输出\t\t\t\t") \
    MACRO(UTXOP2PKH, "未花费公钥HASH输出\t\t\t") \
    MACRO(UTXOP2SH, "未花费脚本HASH输出\t\t\t") \
    MACRO(UTXOMS, "未花费多重签名输出\t\t\t") \
    MACRO(UTXONonStandard, "未花费非标准输出\t\t\t") \
    MACRO(UTXOBAD, "未花费输出脚本错误\t\t\t") \
    \
    MACRO(BadScript, "脚本错误交易\t\t\t\t") \
    MACRO(NonStandard, "非标准交易\t\t\t\t") \
    MACRO(P2PK, "标准公钥交易\t\t\t\t") \
    MACRO(P2PKBadInput, "标准公钥交易<非标准解锁>\t\t") \
    MACRO(P2PKH, "标准公钥HASH交易\t\t\t") \
    MACRO(P2PKHBadInput, "标准公钥HASH交易<非标准解锁>\t\t") \
    MACRO(MS, "标准多重签名交易\t\t\t") \
    MACRO(MSBadInput, "标准多重签名交易<非标准解锁>\t\t") \
    \
    MACRO(P2SH_P2PK, "P2SH标准公钥交易\t\t\t") \
    MACRO(P2SH_P2PKBadInput, "P2SH标准公钥交易<非标准解锁>\t\t") \
    MACRO(P2SH_P2PKH, "P2SH标准公钥HASH交易\t\t\t") \
    MACRO(P2SH_P2PKHBadInput, "P2SH标准公钥HASH交易<非标准解锁>\t") \
    MACRO(P2SH_MS, "P2SH标准多重签名交易\t\t\t") \
    MACRO(P2SH_MSBadInput, "P2SH标准多重签名交易<非标准解锁>\t") \
    MACRO(P2SH_NonStandard, "P2SH非标准交易\t\t\t\t") \
    MACRO(P2SH_BadScript, "P2SH非标准交易\t\t\t\t") \

class StatisticsPool : public OutputPool {
protected:

    template<typename T> std::string _CalcStr(T value, const char *fmt) {
        int integer = (int)value;
        int decimal = ((int)(value * 100.0)) % 100;
        char buffer[32];
        snprintf(buffer, sizeof(buffer), fmt, integer, decimal);
        return std::string(buffer);
    }

    std::string _CalcP(const std::atomic<uint64_t> &base, const std::atomic<uint64_t> &total) {
        return _CalcStr(((double)base / (double)total) * 100.0, "%02d.%02d");
    }
    std::string _CalcByte(const std::atomic<uint64_t> &byte) {
        static const char *unit[] = {
            "B ",
            "K",
            "M",
            "G",
            "T"
        };
        double value = (double)byte;
        int selectUnit = 0;
        while (value >= 1000.0) {
            value /= 1024.0;
            selectUnit++;
        }
        return _CalcStr(value, "%03d.%02d") + std::string(unit[selectUnit]);
    }

#define SCAN_STAT_ADD(name, bytes) m##name##byte += (bytes); m##name##count++

    // define m##xxx##byte m##xxx##count
#define SCAN_STAT_DEFINE___(name, msg) std::atomic<uint64_t> m##name##byte; std::atomic<uint64_t> m##name##count;
    STATISTICS_LIST(SCAN_STAT_DEFINE___)
#undef SCAN_STAT_DEFINE___
    void _Dump() {
        std::atomic<uint64_t> totalByte(0);
        std::atomic<uint64_t> totalCount(0);
        // calc total
#define SCAN_STAT_DEFINE___(name, msg) totalByte += m##name##byte; totalCount += m##name##count;
    STATISTICS_LIST(SCAN_STAT_DEFINE___)
#undef SCAN_STAT_DEFINE___
        // write log
#define SCAN_STAT_DEFINE___(name, msg) if (m##name##count != 0) { \
        BKINFO() << msg << \
                            "字节:" << _CalcP(m##name##byte, totalByte) << "%" << "," << _CalcByte(m##name##byte) << \
                            "\t\t数量:" << _CalcP(m##name##count, totalCount) << "%" << "," << m##name##count;}
    STATISTICS_LIST(SCAN_STAT_DEFINE___)
#undef SCAN_STAT_DEFINE___
        BKINFO() << "总大小\t\t\t\t" << _CalcByte(totalByte);
        BKINFO() << "总数量\t\t\t\t" << totalCount;
    }
    void _Init() {
#define SCAN_STAT_DEFINE___(name, msg) m##name##byte = 0; m##name##count = 0;
    STATISTICS_LIST(SCAN_STAT_DEFINE___)
#undef SCAN_STAT_DEFINE___
    }
public:
    StatisticsPool() {
        _Init();
    }
    ~StatisticsPool() {
        _Dump();
    }
    virtual bool IsUnspendable(const std::vector<uint8_t> &script, int64_t amount) {
        return !script.empty() && script[0] == OP_RETURN;
    }
    virtual void OnOutputUnspendable(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock) {
        SCAN_STAT_ADD(Unspendable, lock.size());
    }
    virtual void OnTransaction(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock, const std::vector<uint8_t> &unlock) {
        std::vector<std::vector<uint8_t>> address;
        auto type = script::GetOutputType(lock, &address);
        auto scriptSize = lock.size() + unlock.size();
        switch (type) {
        case script::Type::ScriptHash:
            switch (script::GetInputType(unlock, type)) {
                case script::Type::PubKey:
                    SCAN_STAT_ADD(P2SH_P2PK, scriptSize);
                    break;
                case script::Type::PubKeyBadInput:
                    SCAN_STAT_ADD(P2SH_P2PKBadInput, scriptSize);
                    break;
                case script::Type::PubKeyHashBadInput:
                    SCAN_STAT_ADD(P2SH_P2PKHBadInput, scriptSize);
                    break;
                case script::Type::MultiSigBadInput:
                    SCAN_STAT_ADD(P2SH_MSBadInput, scriptSize);
                    break;
                case script::Type::PubKeyHash:
                    SCAN_STAT_ADD(P2SH_P2PKH, scriptSize);
                    break;
                case script::Type::MultiSig:
                    SCAN_STAT_ADD(P2SH_MS, scriptSize);
                    break;
                case script::Type::BadScript:
                    SCAN_STAT_ADD(P2SH_BadScript, scriptSize);
                    break;
                default:
                    SCAN_STAT_ADD(P2SH_NonStandard, scriptSize);
                    break;
            }
            break;
        case script::Type::PubKey:
            if (script::GetInputType(unlock, type) != type) {
                SCAN_STAT_ADD(P2PKBadInput, scriptSize);
            } else {
                SCAN_STAT_ADD(P2PK, scriptSize);
            }
            break;
        case script::Type::MultiSig:
            if (script::GetInputType(unlock, type) != type) {
                SCAN_STAT_ADD(MSBadInput, scriptSize);
            } else {
                SCAN_STAT_ADD(MS, scriptSize);
            }
            break;
        case script::Type::PubKeyHash:
            if (script::GetInputType(unlock, type) != type) {
                SCAN_STAT_ADD(P2PKHBadInput, scriptSize);
            } else {
                SCAN_STAT_ADD(P2PKH, scriptSize);
            }
            break;
        case script::Type::NonStandard:
            SCAN_STAT_ADD(NonStandard, scriptSize);
            break;
        default:
            SCAN_STAT_ADD(BadScript, scriptSize);
            break;
        }
    }
    // Main Thread
    virtual void OnInput(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &unlock) {
        SCAN_STAT_ADD(BadInput, unlock.size());
    }
    virtual void OnOutput(const bkbase::Hash256 &txid, uint16_t index, const std::vector<uint8_t> &lock) {
        std::vector<std::vector<uint8_t>> address;
        auto type = script::GetOutputType(lock, &address);
        switch (type) {
        case script::Type::NonStandard:
            SCAN_STAT_ADD(UTXONonStandard, lock.size());
            break;
        case script::Type::PubKey:
            SCAN_STAT_ADD(UTXOP2PK, lock.size());
            break;
        case script::Type::PubKeyHash:
            SCAN_STAT_ADD(UTXOP2PKH, lock.size());
            break;
        case script::Type::MultiSig:
            SCAN_STAT_ADD(UTXOMS, lock.size());
            break;
        case script::Type::ScriptHash:
            SCAN_STAT_ADD(UTXOP2SH, lock.size());
            break;
        default:
            SCAN_STAT_ADD(UTXOBAD, lock.size());
            break;
        }
    }
};

}