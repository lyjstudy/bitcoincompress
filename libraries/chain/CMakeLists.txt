
# chain library
add_library(chain
    src/block.cpp
)

target_link_libraries(chain bkbase)
target_include_directories(chain PUBLIC include)
cotire(chain)

# chain unit tests
if (BITCOINKEYS_BUILD_TESTS)
    bitcoinkeys_test_suite(chain
        tests/main.cpp
        tests/block_tests.cpp
    )
endif (BITCOINKEYS_BUILD_TESTS)

