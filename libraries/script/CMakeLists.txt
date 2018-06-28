
# script library
add_library(script
    src/parser.cpp
    src/type.cpp
    src/opcode.cpp
)

target_link_libraries(script chain)
target_include_directories(script PUBLIC include)
cotire(script)

# script unit tests
if (BITCOINKEYS_BUILD_TESTS)
    bitcoinkeys_test_suite(script
        tests/main.cpp
        tests/parser_tests.cpp
    )
endif (BITCOINKEYS_BUILD_TESTS)


