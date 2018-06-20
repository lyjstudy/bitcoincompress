
# core library
add_library(core
    src/block.cpp
)

target_link_libraries(core bkbase)
target_include_directories(core PUBLIC include)

# core unit tests
if (BITCOINKEYS_BUILD_TESTS)
    bitcoinkeys_test_suite(core
        tests/main.cpp
        tests/block_tests.cpp
    )
endif (BITCOINKEYS_BUILD_TESTS)

