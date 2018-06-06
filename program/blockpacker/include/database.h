#pragma once

#include <stdint.h>
#include <vector>

class PubKeyDB {
public:
    // 如果已经存在只返回索引
    uint64_t Add(const std::vector<uint8_t> &pubkey);
    // 如果公钥HASH已经存在则不需要存储到PubKeyHashDB中
    // 公钥有非压缩格式和压缩格式
    uint64_t FindHash(const std::vector<uint8_t> &pubkeyhash);
};

class PubKeyHashDB {
public:
    uint64_t Add(const std::vector<uint8_t> &pubkeyhash);
};

class ScriptDB {
public:
    uint64_t Add(const std::vector<uint8_t> &script);
    uint64_t FindHash(const std::vector<uint8_t> &scripthash, const std::vector<std::vector<uint8_t>> &pushData);
};

class ScriptHashDB {
public:
    uint64_t Add(const std::vector<uint8_t> &scripthash);
};