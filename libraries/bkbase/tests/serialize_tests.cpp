#include <bkbase/serialize.h>
#include <bkbase/stream.h>
#include <boost/test/unit_test.hpp>

#include <stdint.h>
#include <iostream>
#include <algorithm>

BOOST_AUTO_TEST_SUITE(serialize_tests)

class BaseObject {
    ADD_SERIALIZE_METHODS
    template<typename Stream, typename Action>
    void SerializationOp(Stream &s, const Action &serAction) {
        READWRITE(mData);
        READWRITE(mValue);
    }
public:
    uint16_t mData;
    uint32_t mValue;
    template<typename Stream>
    BaseObject(bkbase::deserialize_type, Stream &s) {
        Unserialize(s);
    }
    BaseObject() {}
    BaseObject(uint16_t data, uint32_t value) : mData(data), mValue(value) {}

    bool operator==(const BaseObject &other) {
        return other.mData == mData && other.mValue == mValue;
    }
};

BOOST_AUTO_TEST_CASE(base) {
    uint8_t buffer[] = {
        0x00, 0x01,
        0x02, 0x03, 0x04, 0x05,
    };
    BaseObject obj(0x0100, 0x05040302);
    BaseObject obj2;

    bkbase::StreamReadPtr readStream(0, 0, buffer, sizeof(buffer));
    readStream >> obj2;
    BOOST_CHECK(obj == obj2);

    bkbase::StreamData dataStream(0, 0);
    dataStream << obj;
    BOOST_CHECK_EQUAL(dataStream.Size(), sizeof(buffer));
    BOOST_CHECK(std::equal(buffer, buffer + sizeof(buffer), dataStream.Buffer().begin()));
    dataStream >> obj;
    BOOST_CHECK(obj == obj2);
    BOOST_CHECK_EQUAL(bkbase::GetSerializeSize(obj, 0), sizeof(buffer));

    BOOST_CHECK_THROW(dataStream >> obj, bkbase::SerializeException);
    try {
        dataStream << obj2.mData;
        dataStream >> obj;
    } catch (bkbase::SerializeException &ex) {
        BOOST_CHECK(boost::get_error_info<bkbase::SerializeError>(ex));
        std::cerr << boost::diagnostic_information(ex) << std::endl;
    }

    readStream.Rewind();
    std::shared_ptr<const BaseObject> objPtr;
    readStream >> objPtr;
    BOOST_CHECK(obj == *objPtr);
    dataStream.Clear();
    dataStream << objPtr;
    BOOST_CHECK_EQUAL(dataStream.Size(), sizeof(buffer));
    BOOST_CHECK(std::equal(buffer, buffer + sizeof(buffer), dataStream.Buffer().begin()));
}

BOOST_AUTO_TEST_CASE(compactsize) {
    ::bkbase::StreamData ss(0, 0);
    std::vector<char>::size_type i, j;

    for (i = 1; i <= bkbase::COMPACT_SIZE_MAX; i *= 2) {
        bkbase::WriteCompactSize(ss, i - 1);
        bkbase::WriteCompactSize(ss, i);
    }
    for (i = 1; i <= bkbase::COMPACT_SIZE_MAX; i *= 2) {
        j = bkbase::ReadCompactSize(ss);
        BOOST_CHECK_MESSAGE((i - 1) == j,
                            "decoded:" << j << " expected:" << (i - 1));
        j = bkbase::ReadCompactSize(ss);
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }

    bkbase::WriteCompactSize(ss, bkbase::COMPACT_SIZE_MAX);
    BOOST_CHECK_EQUAL(bkbase::ReadCompactSize(ss), bkbase::COMPACT_SIZE_MAX);

    bkbase::WriteCompactSize(ss, bkbase::COMPACT_SIZE_MAX + 1);
    BOOST_CHECK_THROW(bkbase::ReadCompactSize(ss), bkbase::SerializeException);

    bkbase::WriteCompactSize(ss, std::numeric_limits<int64_t>::max());
    BOOST_CHECK_THROW(bkbase::ReadCompactSize(ss), bkbase::SerializeException);

    bkbase::WriteCompactSize(ss, std::numeric_limits<uint64_t>::max());
    BOOST_CHECK_THROW(bkbase::ReadCompactSize(ss), bkbase::SerializeException);

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
        ss << ::bkbase::CompactSize<uint64_t>(test_list[i].value);

        BOOST_CHECK_EQUAL(buffer.size(), test_list[i].buffer.size());
        BOOST_CHECK_EQUAL(memcmp(&buffer[0], &test_list[i].buffer[0], buffer.size()), 0);

        if (test_list[i].value <= bkbase::COMPACT_SIZE_MAX) {
            uint64_t outVal;
            ::bkbase::CompactSize<uint64_t> compact(outVal);
            ss >> compact;
            BOOST_CHECK_EQUAL(outVal, test_list[i].value);
        }
    }

}

BOOST_AUTO_TEST_CASE(noncanonical) {
    // Write some non-canonical CompactSize encodings, and make sure an
    // exception is thrown when read back.
    ::bkbase::StreamData ss(0, 0);
    size_t n;

    // zero encoded with three bytes:
    ss.Write("\xfd\x00\x00", 3);
    BOOST_CHECK_THROW(bkbase::ReadCompactSize(ss), bkbase::SerializeException);

    // 0xfc encoded with three bytes:
    ss.Write("\xfd\xfc\x00", 3);
    BOOST_CHECK_THROW(bkbase::ReadCompactSize(ss), bkbase::SerializeException);

    // 0xfd encoded with three bytes is OK:
    ss.Write("\xfd\xfd\x00", 3);
    n = bkbase::ReadCompactSize(ss);
    BOOST_CHECK(n == 0xfd);

    // zero encoded with five bytes:
    ss.Write("\xfe\x00\x00\x00\x00", 5);
    BOOST_CHECK_THROW(bkbase::ReadCompactSize(ss), bkbase::SerializeException);

    // 0xffff encoded with five bytes:
    ss.Write("\xfe\xff\xff\x00\x00", 5);
    BOOST_CHECK_THROW(bkbase::ReadCompactSize(ss), bkbase::SerializeException);

    // zero encoded with nine bytes:
    ss.Write("\xff\x00\x00\x00\x00\x00\x00\x00\x00", 9);
    BOOST_CHECK_THROW(bkbase::ReadCompactSize(ss), bkbase::SerializeException);

    // 0x01ffffff encoded with nine bytes:
    ss.Write("\xff\xff\xff\xff\x01\x00\x00\x00\x00", 9);
    BOOST_CHECK_THROW(bkbase::ReadCompactSize(ss), bkbase::SerializeException);
}

BOOST_AUTO_TEST_CASE(varints) {
    // encode

    ::bkbase::StreamData ss(0, 0);
    for (int i = 0; i < 100000; i++) {
        ss << ::bkbase::VarInt<int>(i);
    }

    for (uint64_t i = 0; i < 100000000000ULL; i += 999999937) {
        ss << ::bkbase::VarInt<uint64_t>(i);
    }

    // decode
    for (int i = 0; i < 100000; i++) {
        int j = -1;
        ::bkbase::VarInt<int> varint(j);
        ss >> varint;
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }

    for (uint64_t i = 0; i < 100000000000ULL; i += 999999937) {
        uint64_t j = -1;
        ::bkbase::VarInt<uint64_t> varint(j);
        ss >> varint;
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }
}


BOOST_AUTO_TEST_SUITE_END()
