
add_library(script
    src/parser.cpp
    src/type.cpp
    src/opcode.cpp
    src/signature.cpp
)


target_link_libraries(script core)

target_include_directories(script PUBLIC core include)

create_test_suite(script)
add_test_to_suite(script test_script
    tests/parser_tests.cpp
)

target_include_directories(test_script
    PUBLIC include
)

find_package(Boost 1.58 REQUIRED unit_test_framework)
target_link_libraries(test_script Boost::unit_test_framework script)

# We need to detect if the BOOST_TEST_DYN_LINK flag is required.
set(CMAKE_REQUIRED_LIBRARIES Boost::unit_test_framework)
check_cxx_source_compiles("
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
" BOOST_TEST_DYN_LINK)

if(BOOST_TEST_DYN_LINK)
target_compile_definitions(test_script PRIVATE BOOST_TEST_DYN_LINK)
endif(BOOST_TEST_DYN_LINK)

