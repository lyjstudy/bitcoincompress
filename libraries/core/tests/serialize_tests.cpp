// Copyright (c) 2012-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#define BOOST_TEST_MAIN
#include "serialize.h"

#include <cstdint>
#include <limits>

#include <boost/test/unit_test.hpp>

using namespace core;

template<typename To, typename From>
static To ser_to(From fromValue) {
    To value;
    DataStream stream(0, 0);
    ::core::Serialize(stream, fromValue);
    ::core::Unserialize(stream, value);
    return value;
}

static float ser_uint32_to_float(uint32_t value) { return ser_to<float, uint32_t>(value); }
static uint32_t ser_float_to_uint32(float value) { return ser_to<uint32_t, float>(value); }
static double ser_uint64_to_double(uint64_t value) { return ser_to<double, uint64_t>(value); }
static uint64_t ser_double_to_uint64(double value) { return ser_to<uint64_t, double>(value); }

static std::string HexStr(DataStream &stream) {
    std::string value;
    char str[32];
    auto &buffer = stream.Buffer();
    for (auto val : buffer) {
        sprintf(str, "%02x", (uint8_t)val);
        value += str;
    }
    return value;
}

BOOST_AUTO_TEST_SUITE(serialize_tests)

BOOST_AUTO_TEST_CASE(sizes) {
    BOOST_CHECK_EQUAL(sizeof(char), GetSerializeSize(char(0), 0));
    BOOST_CHECK_EQUAL(sizeof(int8_t), GetSerializeSize(int8_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(uint8_t), GetSerializeSize(uint8_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(int16_t), GetSerializeSize(int16_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(uint16_t), GetSerializeSize(uint16_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(int32_t), GetSerializeSize(int32_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(uint32_t), GetSerializeSize(uint32_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(int64_t), GetSerializeSize(int64_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(uint64_t), GetSerializeSize(uint64_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(float), GetSerializeSize(float(0), 0));
    BOOST_CHECK_EQUAL(sizeof(double), GetSerializeSize(double(0), 0));
    // Bool is serialized as char
    BOOST_CHECK_EQUAL(sizeof(char), GetSerializeSize(bool(0), 0));

    // Sanity-check GetSerializeSize and c++ type matching
    BOOST_CHECK_EQUAL(GetSerializeSize(char(0), 0), 1);
    BOOST_CHECK_EQUAL(GetSerializeSize(int8_t(0), 0), 1);
    BOOST_CHECK_EQUAL(GetSerializeSize(uint8_t(0), 0), 1);
    BOOST_CHECK_EQUAL(GetSerializeSize(int16_t(0), 0), 2);
    BOOST_CHECK_EQUAL(GetSerializeSize(uint16_t(0), 0), 2);
    BOOST_CHECK_EQUAL(GetSerializeSize(int32_t(0), 0), 4);
    BOOST_CHECK_EQUAL(GetSerializeSize(uint32_t(0), 0), 4);
    BOOST_CHECK_EQUAL(GetSerializeSize(int64_t(0), 0), 8);
    BOOST_CHECK_EQUAL(GetSerializeSize(uint64_t(0), 0), 8);
    BOOST_CHECK_EQUAL(GetSerializeSize(float(0), 0), 4);
    BOOST_CHECK_EQUAL(GetSerializeSize(double(0), 0), 8);
    BOOST_CHECK_EQUAL(GetSerializeSize(bool(0), 0), 1);
}

BOOST_AUTO_TEST_CASE(floats_conversion) {
    // Choose values that map unambiguously to binary floating point to avoid
    // rounding issues at the compiler side.
    BOOST_CHECK_EQUAL(ser_uint32_to_float(0x00000000), 0.0F);
    BOOST_CHECK_EQUAL(ser_uint32_to_float(0x3f000000), 0.5F);
    BOOST_CHECK_EQUAL(ser_uint32_to_float(0x3f800000), 1.0F);
    BOOST_CHECK_EQUAL(ser_uint32_to_float(0x40000000), 2.0F);
    BOOST_CHECK_EQUAL(ser_uint32_to_float(0x40800000), 4.0F);
    BOOST_CHECK_EQUAL(ser_uint32_to_float(0x44444444), 785.066650390625F);

    BOOST_CHECK_EQUAL(ser_float_to_uint32(0.0F), 0x00000000);
    BOOST_CHECK_EQUAL(ser_float_to_uint32(0.5F), 0x3f000000);
    BOOST_CHECK_EQUAL(ser_float_to_uint32(1.0F), 0x3f800000);
    BOOST_CHECK_EQUAL(ser_float_to_uint32(2.0F), 0x40000000);
    BOOST_CHECK_EQUAL(ser_float_to_uint32(4.0F), 0x40800000);
    BOOST_CHECK_EQUAL(ser_float_to_uint32(785.066650390625F), 0x44444444);
}

BOOST_AUTO_TEST_CASE(doubles_conversion) {
    // Choose values that map unambiguously to binary floating point to avoid
    // rounding issues at the compiler side.
    BOOST_CHECK_EQUAL(ser_uint64_to_double(0x0000000000000000ULL), 0.0);
    BOOST_CHECK_EQUAL(ser_uint64_to_double(0x3fe0000000000000ULL), 0.5);
    BOOST_CHECK_EQUAL(ser_uint64_to_double(0x3ff0000000000000ULL), 1.0);
    BOOST_CHECK_EQUAL(ser_uint64_to_double(0x4000000000000000ULL), 2.0);
    BOOST_CHECK_EQUAL(ser_uint64_to_double(0x4010000000000000ULL), 4.0);
    BOOST_CHECK_EQUAL(ser_uint64_to_double(0x4088888880000000ULL),
                      785.066650390625);

    BOOST_CHECK_EQUAL(ser_double_to_uint64(0.0), 0x0000000000000000ULL);
    BOOST_CHECK_EQUAL(ser_double_to_uint64(0.5), 0x3fe0000000000000ULL);
    BOOST_CHECK_EQUAL(ser_double_to_uint64(1.0), 0x3ff0000000000000ULL);
    BOOST_CHECK_EQUAL(ser_double_to_uint64(2.0), 0x4000000000000000ULL);
    BOOST_CHECK_EQUAL(ser_double_to_uint64(4.0), 0x4010000000000000ULL);
    BOOST_CHECK_EQUAL(ser_double_to_uint64(785.066650390625),
                      0x4088888880000000ULL);
}

// TODO: Hash
#if 0
/*
Python code to generate the below hashes:

    def reversed_hex(x):
        return binascii.hexlify(''.join(reversed(x)))
    def dsha256(x):
        return hashlib.sha256(hashlib.sha256(x).digest()).digest()

    reversed_hex(dsha256(''.join(struct.pack('<f', x) for x in range(0,1000))))
== '8e8b4cf3e4df8b332057e3e23af42ebc663b61e0495d5e7e32d85099d7f3fe0c'
    reversed_hex(dsha256(''.join(struct.pack('<d', x) for x in range(0,1000))))
== '43d0c82591953c4eafe114590d392676a01585d25b25d433557f0d7878b23f96'
*/
BOOST_AUTO_TEST_CASE(floats) {
    CDataStream ss(SER_DISK, 0);
    // encode
    for (int i = 0; i < 1000; i++) {
        ss << float(i);
    }
    BOOST_CHECK(Hash(ss.begin(), ss.end()) ==
                uint256S("8e8b4cf3e4df8b332057e3e23af42ebc663b61e0495d5e7e32d85"
                         "099d7f3fe0c"));

    // decode
    for (int i = 0; i < 1000; i++) {
        float j;
        ss >> j;
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }
}

BOOST_AUTO_TEST_CASE(doubles) {
    CDataStream ss(SER_DISK, 0);
    // encode
    for (int i = 0; i < 1000; i++) {
        ss << double(i);
    }
    BOOST_CHECK(Hash(ss.begin(), ss.end()) ==
                uint256S("43d0c82591953c4eafe114590d392676a01585d25b25d433557f0"
                         "d7878b23f96"));

    // decode
    for (int i = 0; i < 1000; i++) {
        double j;
        ss >> j;
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }
}
#endif


BOOST_AUTO_TEST_CASE(varints) {
    // encode

    DataStream ss(0, 0);
    for (int i = 0; i < 100000; i++) {
        SerializeVarInt(ss, i);
    }

    for (uint64_t i = 0; i < 100000000000ULL; i += 999999937) {
        SerializeVarInt(ss, i);
    }

    // decode
    for (int i = 0; i < 100000; i++) {
        int j = -1;
        UnserializeVarInt(ss, j);
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }

    for (uint64_t i = 0; i < 100000000000ULL; i += 999999937) {
        uint64_t j = -1;
        UnserializeVarInt(ss, j);
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }
}

BOOST_AUTO_TEST_CASE(varints_bitpatterns) {
    DataStream ss(0, 0);
    SerializeVarInt(ss, 0);
    BOOST_CHECK_EQUAL(HexStr(ss), "00");
    ss.Clear();
    SerializeVarInt(ss, 0x7f);
    BOOST_CHECK_EQUAL(HexStr(ss), "7f");
    ss.Clear();
    SerializeVarInt(ss, (int8_t)0x7f);
    BOOST_CHECK_EQUAL(HexStr(ss), "7f");
    ss.Clear();
    SerializeVarInt(ss, 0x80);
    BOOST_CHECK_EQUAL(HexStr(ss), "8000");
    ss.Clear();
    SerializeVarInt(ss, (uint8_t)0x80);
    BOOST_CHECK_EQUAL(HexStr(ss), "8000");
    ss.Clear();
    SerializeVarInt(ss, 0x1234);
    BOOST_CHECK_EQUAL(HexStr(ss), "a334");
    ss.Clear();
    SerializeVarInt(ss, (int16_t)0x1234);
    BOOST_CHECK_EQUAL(HexStr(ss), "a334");
    ss.Clear();
    SerializeVarInt(ss, 0xffff);
    BOOST_CHECK_EQUAL(HexStr(ss), "82fe7f");
    ss.Clear();
    SerializeVarInt(ss, (uint16_t)0xffff);
    BOOST_CHECK_EQUAL(HexStr(ss), "82fe7f");
    ss.Clear();
    SerializeVarInt(ss, 0x123456);
    BOOST_CHECK_EQUAL(HexStr(ss), "c7e756");
    ss.Clear();
    SerializeVarInt(ss, (int32_t)0x123456);
    BOOST_CHECK_EQUAL(HexStr(ss), "c7e756");
    ss.Clear();
    SerializeVarInt(ss, 0x80123456U);
    BOOST_CHECK_EQUAL(HexStr(ss), "86ffc7e756");
    ss.Clear();
    SerializeVarInt(ss, (uint32_t)0x80123456U);
    BOOST_CHECK_EQUAL(HexStr(ss), "86ffc7e756");
    ss.Clear();
    SerializeVarInt(ss, 0xffffffff);
    BOOST_CHECK_EQUAL(HexStr(ss), "8efefefe7f");
    ss.Clear();
    SerializeVarInt(ss, (int64_t)0x7fffffffffffffffLL);
    BOOST_CHECK_EQUAL(HexStr(ss), "fefefefefefefefe7f");
    ss.Clear();
    SerializeVarInt(ss, (uint64_t)0xffffffffffffffffULL);
    BOOST_CHECK_EQUAL(HexStr(ss), "80fefefefefefefefe7f");
    ss.Clear();

    struct {
        uint64_t value;
        std::vector<uint8_t> buffer;
    } test_list[] = {
        {0,              {0x00}},
        {1,              {0x01}},
        {127,            {0x7F}},
        {128,            {0x80, 0x00}},
        {255,            {0x80, 0x7F}},
        {256,            {0x81, 0x00}},
        {16383,          {0xFE, 0x7F}},
        {16384,          {0xFF, 0x00}},
        {16511,          {0xFF, 0x7F}},
        {65535,          {0x82, 0xFE, 0x7F}},
        {4294967296,     {0x8E, 0xFE, 0xFE, 0xFF, 0x00}},
        {0x00,           {0x00}},
        {0x7F,           {0x7F}},
        {0x80,           {0x80, 0x00}},
        {0x407F,         {0xFF, 0x7F}},
        {0x4080,         {0x80, 0x80, 0x00}},
        {0x20407F,       {0xFF, 0xFF, 0x7F}},
        {0x204080,       {0x80, 0x80, 0x80, 0x00}},
        {0x1020407F,     {0xFF, 0xFF, 0xFF, 0x7F}},
        {0x10204080,     {0x80, 0x80, 0x80, 0x80, 0x00}},
        {0x81020407F,    {0xFF, 0xFF, 0xFF, 0xFF, 0x7F}},
        {0x810204080,    {0x80, 0x80, 0x80, 0x80, 0x80, 0x00}},
        {0x4081020407F,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F}},
        {0x40810204080,  {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00}},
    };
    for (size_t i = 0; i < sizeof(test_list) / sizeof(test_list[0]); i++) {
        auto &buffer = ss.Buffer();
        ss.Clear();
        ss.Rewind();
        SerializeVarInt(ss, test_list[i].value);

        BOOST_CHECK_EQUAL(buffer.size(), test_list[i].buffer.size());
        BOOST_CHECK_EQUAL(memcmp(&buffer[0], &test_list[i].buffer[0], buffer.size()), 0);

        uint64_t outVal;
        BOOST_CHECK_EQUAL(UnserializeVarInt(ss, outVal), true);
        BOOST_CHECK_EQUAL(outVal, test_list[i].value);
    }
}

template<typename T>
void WriteCompactSize(DataStream &ss, T value) {
    SerializeCompactSize(ss, value);
}

std::vector<char>::size_type ReadCompactSize(DataStream &ss) {
    std::vector<char>::size_type value;
    if (!UnserializeCompactSize(ss, value)) return -1;
    return value;
}

BOOST_AUTO_TEST_CASE(compactsize) {
    DataStream ss(0, 0);
    std::vector<char>::size_type i, j;

    for (i = 1; i <= COMPACT_SIZE_MAX; i *= 2) {
        WriteCompactSize(ss, i - 1);
        WriteCompactSize(ss, i);
    }
    for (i = 1; i <= COMPACT_SIZE_MAX; i *= 2) {
        j = ReadCompactSize(ss);
        BOOST_CHECK_MESSAGE((i - 1) == j,
                            "decoded:" << j << " expected:" << (i - 1));
        j = ReadCompactSize(ss);
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }

    WriteCompactSize(ss, COMPACT_SIZE_MAX);
    BOOST_CHECK_EQUAL(ReadCompactSize(ss), COMPACT_SIZE_MAX);

    WriteCompactSize(ss, COMPACT_SIZE_MAX + 1);
    BOOST_CHECK_EQUAL(ReadCompactSize(ss), -1);

    WriteCompactSize(ss, std::numeric_limits<int64_t>::max());
    BOOST_CHECK_EQUAL(ReadCompactSize(ss), -1);

    WriteCompactSize(ss, std::numeric_limits<uint64_t>::max());
    BOOST_CHECK_EQUAL(ReadCompactSize(ss), -1);

    struct {
        uint64_t value;
        std::vector<uint8_t> buffer;
    } test_list[] = {
        {0,                     {0x00}},
        {0xFC,                  {0xFC}},
        {0xFD,                  {0xFD, 0xFD, 0x00}},
        {0xFFFF,                {0xFD, 0xFF, 0xFF}},
        {0x10000,               {0xFE, 0x00, 0x00, 0x01, 0x00}},
        {0xFFFFFFFF,            {0xFE, 0xFF, 0xFF, 0xFF, 0xFF}},
        {0x100000000,           {0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}},
        {0xFFFFFFFFFFFFFFFF,    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    };

    for (size_t i = 0; i < sizeof(test_list) / sizeof(test_list[0]); i++) {
        
        auto &buffer = ss.Buffer();
        ss.Clear();
        ss.Rewind();
        SerializeCompactSize(ss, test_list[i].value);

        BOOST_CHECK_EQUAL(buffer.size(), test_list[i].buffer.size());
        BOOST_CHECK_EQUAL(memcmp(&buffer[0], &test_list[i].buffer[0], buffer.size()), 0);

        if (test_list[i].value <= COMPACT_SIZE_MAX) {
            uint64_t outVal;
            BOOST_CHECK_EQUAL(UnserializeCompactSize(ss, outVal), true);
            BOOST_CHECK_EQUAL(outVal, test_list[i].value);
        }
    }
}

BOOST_AUTO_TEST_CASE(noncanonical) {
    // Write some non-canonical CompactSize encodings, and make sure an
    // exception is thrown when read back.
    DataStream ss(0, 0);
    std::vector<char>::size_type n;

    // zero encoded with three bytes:
    ss.Write("\xfd\x00\x00", 3);
    BOOST_CHECK_EQUAL(ReadCompactSize(ss), -1);

    // 0xfc encoded with three bytes:
    ss.Write("\xfd\xfc\x00", 3);
    BOOST_CHECK_EQUAL(ReadCompactSize(ss), -1);

    // 0xfd encoded with three bytes is OK:
    ss.Write("\xfd\xfd\x00", 3);
    n = ReadCompactSize(ss);
    BOOST_CHECK(n == 0xfd);

    // zero encoded with five bytes:
    ss.Write("\xfe\x00\x00\x00\x00", 5);
    BOOST_CHECK_EQUAL(ReadCompactSize(ss), -1);

    // 0xffff encoded with five bytes:
    ss.Write("\xfe\xff\xff\x00\x00", 5);
    BOOST_CHECK_EQUAL(ReadCompactSize(ss), -1);

    // zero encoded with nine bytes:
    ss.Write("\xff\x00\x00\x00\x00\x00\x00\x00\x00", 9);
    BOOST_CHECK_EQUAL(ReadCompactSize(ss), -1);

    // 0x01ffffff encoded with nine bytes:
    ss.Write("\xff\xff\xff\xff\x01\x00\x00\x00\x00", 9);
    BOOST_CHECK_EQUAL(ReadCompactSize(ss), -1);
}

class TestObject {
    ADD_SERIALIZE_METHODS;
public:
    uint8_t id;
    uint64_t val;
    uint32_t longdata;
    uint32_t count;
    TestObject() {
        id = 0;
        val = 0;
        longdata = 0;
        count = 0;
    }
    template<typename Stream, typename Operation>
    bool SerializationOp(Stream &s, Operation serAction) {
        READWRITE(id);
        READWRITE(val);
        READWRITEVARINT(longdata);
        READWRITECOMPACT(count);
        return true;
    }
};

BOOST_AUTO_TEST_CASE(class_methods) {
    TestObject obj, obj2;
    obj.id = 10;
    obj.val = 10000;
    obj.longdata = 2020887;
    obj.count = 778899;
    DataStream ss(0, 0);
    Serialize(ss, obj);
    Unserialize(ss, obj2);
    BOOST_CHECK_EQUAL(obj2.id, obj.id);
    BOOST_CHECK_EQUAL(obj2.val, obj.val);
    BOOST_CHECK_EQUAL(obj2.longdata, obj.longdata);
    BOOST_CHECK_EQUAL(obj2.count, obj.count);
}

#if 0
BOOST_AUTO_TEST_CASE(insert_delete) {
    // Test inserting/deleting bytes.
    CDataStream ss(SER_DISK, 0);
    BOOST_CHECK_EQUAL(ss.size(), 0);

    ss.write("\x00\x01\x02\xff", 4);
    BOOST_CHECK_EQUAL(ss.size(), 4);

    char c = (char)11;

    // Inserting at beginning/end/middle:
    ss.insert(ss.begin(), c);
    BOOST_CHECK_EQUAL(ss.size(), 5);
    BOOST_CHECK_EQUAL(ss[0], c);
    BOOST_CHECK_EQUAL(ss[1], 0);

    ss.insert(ss.end(), c);
    BOOST_CHECK_EQUAL(ss.size(), 6);
    BOOST_CHECK_EQUAL(ss[4], (char)0xff);
    BOOST_CHECK_EQUAL(ss[5], c);

    ss.insert(ss.begin() + 2, c);
    BOOST_CHECK_EQUAL(ss.size(), 7);
    BOOST_CHECK_EQUAL(ss[2], c);

    // Delete at beginning/end/middle
    ss.erase(ss.begin());
    BOOST_CHECK_EQUAL(ss.size(), 6);
    BOOST_CHECK_EQUAL(ss[0], 0);

    ss.erase(ss.begin() + ss.size() - 1);
    BOOST_CHECK_EQUAL(ss.size(), 5);
    BOOST_CHECK_EQUAL(ss[4], (char)0xff);

    ss.erase(ss.begin() + 1);
    BOOST_CHECK_EQUAL(ss.size(), 4);
    BOOST_CHECK_EQUAL(ss[0], 0);
    BOOST_CHECK_EQUAL(ss[1], 1);
    BOOST_CHECK_EQUAL(ss[2], 2);
    BOOST_CHECK_EQUAL(ss[3], (char)0xff);

    // Make sure GetAndClear does the right thing:
    CSerializeData d;
    ss.GetAndClear(d);
    BOOST_CHECK_EQUAL(ss.size(), 0);
}

BOOST_AUTO_TEST_CASE(class_methods) {
    int intval(100);
    bool boolval(true);
    std::string stringval("testing");
    const char *charstrval("testing charstr");
    CMutableTransaction txval;
    CSerializeMethodsTestSingle methodtest1(intval, boolval, stringval,
                                            charstrval, CTransaction(txval));
    CSerializeMethodsTestMany methodtest2(intval, boolval, stringval,
                                          charstrval, CTransaction(txval));
    CSerializeMethodsTestSingle methodtest3;
    CSerializeMethodsTestMany methodtest4;
    CDataStream ss(SER_DISK, PROTOCOL_VERSION);
    BOOST_CHECK(methodtest1 == methodtest2);
    ss << methodtest1;
    ss >> methodtest4;
    ss << methodtest2;
    ss >> methodtest3;
    BOOST_CHECK(methodtest1 == methodtest2);
    BOOST_CHECK(methodtest2 == methodtest3);
    BOOST_CHECK(methodtest3 == methodtest4);

    CDataStream ss2(SER_DISK, PROTOCOL_VERSION, intval, boolval, stringval,
                    FLATDATA(charstrval), txval);
    ss2 >> methodtest3;
    BOOST_CHECK(methodtest3 == methodtest4);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
