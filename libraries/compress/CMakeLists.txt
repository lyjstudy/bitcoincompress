
# library compress
add_library(compress
    src/signature.cpp
)

target_link_libraries(compress core)
target_include_directories(compress PUBLIC core include)


# library compress unit tests
create_test_suite(compress)
add_test_to_suite(compress test_compress
    tests/main.cpp
    tests/amount_tests.cpp
)
target_include_directories(test_compress PUBLIC include)
find_package(Boost 1.58 REQUIRED unit_test_framework)
target_link_libraries(test_compress Boost::unit_test_framework compress)
target_compile_definitions(test_compress PRIVATE BOOST_TEST_DYN_LINK)