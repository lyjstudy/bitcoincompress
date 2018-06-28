#include <unordered_map>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <script/opcode.h>
#include <bkbase/hex.h>
#include <bkbase/endian.h>
#include <script/exception.h>
#include <bkbase/formats.h>

namespace script {

static std::unordered_map<std::string, uint8_t> OpNameMap = {
#define DEFINE_OP_CODE(name, value) {#name, OP_##name},
OP_CODE_FOR_EACH(DEFINE_OP_CODE)
OP_CODE_ALIAS_FOR_EACH(DEFINE_OP_CODE)
#undef DEFINE_OP_CODE
#define DEFINE_OP_CODE(name, value) {"OP_" #name, OP_##name},
OP_CODE_FOR_EACH(DEFINE_OP_CODE)
OP_CODE_ALIAS_FOR_EACH(DEFINE_OP_CODE)
#undef DEFINE_OP_CODE
    {"-1", OP_1NEGATE},
};

const char *GetOpName(uint8_t opcode) {
    switch (opcode) {
#define DEFINE_OP_CODE(name, value) \
    case OP_##name: return #name;
OP_CODE_FOR_EACH(DEFINE_OP_CODE)
#undef DEFINE_OP_CODE
    default:
        break;
    }
    return "INVALIDOPCODE";
}

uint8_t GetOpValue(const std::string &name) {
    auto finded = OpNameMap.find(name);
    if (finded == OpNameMap.end())
        return OP_INVALIDOPCODE;
    return finded->second;
}

std::string DisAssemble(const std::vector<uint8_t> &codes) {
    return "";
}

static bool IsEmpty(char val) {
    switch (val) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return true;
        default:
            break;
    } 
    return false;
}

static void SkipEmpty(const std::string &codeStr, std::string::const_iterator &pos) {
    while (pos != codeStr.end()) {
        if (!IsEmpty(*pos)) return;
        ++pos;
    }
}

static std::string GetToken(const std::string &codeStr, std::string::const_iterator &pos) {
    std::string token;

    while (pos != codeStr.end()) {
        if (IsEmpty(*pos)) return token;
        token.push_back(*pos);
    }

    return token;
}

static inline bool IsDecimalChar(char val) {
    return val >= '0' && val <= '9';
}
static inline bool IsHexChar(char val) {
    if (val >= '0' && val <= '9') return true;
    if (val >= 'a' && val <= 'f') return true;
    if (val >= 'A' && val <= 'F') return true;
    return false;
}

static bool IsDecimal(const std::string &token) {
    if (std::all_of(token.begin(), token.end(), IsDecimalChar)) return true;
    if (boost::algorithm::starts_with(token, "-")) {
        if (token.length() > 1) {
            if (std::all_of(token.begin() + 1, token.end(), IsDecimalChar)) return true;
        }
    }
    return false;
}
static bool IsHex(const std::string &token) {
    if (boost::algorithm::starts_with(token, "0x")) {
        if (token.length() < 4 || (token.length() % 2) != 0) return false;
        if (std::all_of(token.begin() + 1, token.end(), IsHexChar)) return true;
    }
    return false;
}

static std::vector<uint8_t> ParseToken(const std::string &token, size_t start) {
    /*
    if (IsDecimal(token)) {
        try
        {
            int64_t val = boost::lexical_cast<int64_t>(token);
            
        }
        catch (const boost::bad_lexical_cast&)
        {
            return false;
        }
    } else if (IsHex(token)) {
        std::vector<uint8_t> value = bkbase::HexToBin(token.c_str() + 2);
        if (value.size() == 0) {
            BK_THROW_ASSEMBLE(tokenStart, "Bad hex value");
        } else if (value.size() == 1) {
            if (value[0] == 0x81) codes.push_back(OP_1NEGATE);
            else if (value[0] == 0) codes.push_back(OP_0)
        }
    } else if (token.size() >= 2 && boost::algorithm::starts_with(token, "'") &&
        boost::algorithm::ends_with(token, "'")) {
        // string
    }
    */
    BK_THROW_ASSEMBLE(start, "Bad Script");
}

static void AddPushData(std::vector<uint8_t> &codes, const std::vector<uint8_t> &data) {
    if (data.size() <= 75) {
        codes.push_back((uint8_t)data.size());
    } else if (data.size() <= 255) {
        codes.push_back(OP_PUSHDATA1);
        codes.push_back((uint8_t)data.size());
    } else if (data.size() <= 65535) {
        codes.push_back(OP_PUSHDATA2);
        auto pos = codes.size();
        codes.resize(pos + 2);
        bkbase::WriteLE<uint16_t>(&codes[pos], (uint16_t)codes.size());
    } else {
        codes.push_back(OP_PUSHDATA4);
        auto pos = codes.size();
        codes.resize(pos + 4);
        bkbase::WriteLE<uint32_t>(&codes[pos], (uint32_t)codes.size());
    }
    
}

std::vector<uint8_t> Assemble(const std::string &codeStr) {
    std::vector<uint8_t> codes;

    auto pos = codeStr.begin();

    while (pos != codeStr.end()) {
        SkipEmpty(codeStr, pos);
        if (pos == codeStr.end()) break;
        size_t tokenStart = pos - codeStr.begin();
        std::string token = GetToken(codeStr, pos);
        if (token.empty()) continue;

        auto finded = OpNameMap.find(token);
        if (finded != OpNameMap.end()) {
            codes.push_back(finded->second);
            switch (finded->second) {
                case OP_PUSHDATA1:
                case OP_PUSHDATA2:
                case OP_PUSHDATA4: {
                        tokenStart = pos - codeStr.begin();
                        token = GetToken(codeStr, pos);
                        if (token.empty()) {
                            BK_THROW_ASSEMBLE(tokenStart, "Bad PUSHDATA");
                        }
                        auto data = ParseToken(token, tokenStart);
                        switch (finded->second) {
                            case OP_PUSHDATA1:
                                if (data.size() > 255)
                                    BK_THROW_ASSEMBLE(tokenStart, "Bad PUSHDATA1");
                                break;
                            case OP_PUSHDATA2:
                                if (data.size() > 65535)
                                    BK_THROW_ASSEMBLE(tokenStart, "Bad PUSHDATA2");
                            default:
                                break;
                        }
                        AddPushData(codes, data);
                    }
                    break;
                default:
                    break;
            }
        } else {
            AddPushData(codes, ParseToken(token, tokenStart));
        }
    }

    return codes;
}

} // script

/*
std::string GetOpString(const std::vector<uint8_t> &codes) {
    std::string strCodes;	
	
    script::Parser parser(codes);	
    char buffer[1024];	
	
    uint8_t op;	
    std::vector<uint8_t> data;	
    while (parser.Fetch(op, &data)) {	
        if (op <= OP_PUSHDATA4) {	
            // PUSHDATA(%d)(%s)	
            snprintf(buffer, sizeof(buffer), "PUSHDATA(%d)[%s] ", (int)data.size(), bkbase::HexFromBin(data).c_str());	
        } else {	
            snprintf(buffer, sizeof(buffer), "%s ", GetOpName(op));	
        }	
        if (!strCodes.empty()) strCodes += "\n";
        strCodes += buffer;	
    }	
	
    return strCodes;	
}
*/