
# bkbase library
add_library(bkbase
    src/stream.cpp
    src/hex.cpp
    src/logging.cpp
    src/hash.cpp
)

target_link_libraries(bkbase ${BKBASE_LIBRARIES})
target_include_directories(bkbase PUBLIC ${BKBASE_INCLUDES} include)

# bkbase unit tests
if (BITCOINKEYS_BUILD_TESTS)
    bitcoinkeys_test_suite(bkbase
        tests/main.cpp
        tests/exception_tests.cpp
        tests/endian_tests.cpp
        tests/serialize_tests.cpp
        tests/hex_tests.cpp
        tests/logging_tests.cpp
        tests/hash_tests.cpp
    )
endif (BITCOINKEYS_BUILD_TESTS)

