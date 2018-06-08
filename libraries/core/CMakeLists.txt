
# core library
add_library(core
    src/serialize.cpp
    src/hash.cpp
    src/hex.cpp
    src/transaction.cpp
)

target_link_libraries(core ${CORE_LIBRARIES})
target_include_directories(core PUBLIC ${CORE_INCLUDES} include)

# core unit tests
if (BITCOINKEYS_BUILD_TESTS)
    bitcoinkeys_test_suite(core
        tests/serialize_tests.cpp
        tests/hash_tests.cpp
        tests/hex_tests.cpp
        tests/transaction_tests.cpp
    )
endif (BITCOINKEYS_BUILD_TESTS)

