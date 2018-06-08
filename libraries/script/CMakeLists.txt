
# script library
add_library(script
    src/parser.cpp
    src/type.cpp
    src/opcode.cpp
    src/signature.cpp
)

target_link_libraries(script core)
target_include_directories(script PUBLIC core include)

# script unit tests
if (BITCOINKEYS_BUILD_TESTS)
    bitcoinkeys_test_suite(script
        tests/parser_tests.cpp
    )
endif (BITCOINKEYS_BUILD_TESTS)


