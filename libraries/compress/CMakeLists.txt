
# compress library
add_library(compress
    src/signature.cpp
    src/pubkey.cpp
    src/pubkeydb.cpp
)

target_link_libraries(compress core)
target_include_directories(compress PUBLIC core include)

# compress unit tests
if (BITCOINKEYS_BUILD_TESTS)
    bitcoinkeys_test_suite(compress
        tests/main.cpp
        tests/amount_tests.cpp
        tests/signature_tests.cpp
        tests/pubkey_tests.cpp
        tests/pubkeydb_tests.cpp
    )
endif (BITCOINKEYS_BUILD_TESTS)