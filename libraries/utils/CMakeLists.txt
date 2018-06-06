
add_library(utils
    src/block_loader.cpp
)


target_link_libraries(utils core)

target_include_directories(utils PUBLIC core include)

create_test_suite(utils)
add_test_to_suite(utils test_utils
    tests/block_loader_tests.cpp
)

target_include_directories(test_utils
	PUBLIC include
)

find_package(Boost 1.58 REQUIRED unit_test_framework)
target_link_libraries(test_utils Boost::unit_test_framework utils)

# We need to detect if the BOOST_TEST_DYN_LINK flag is required.
set(CMAKE_REQUIRED_LIBRARIES Boost::unit_test_framework)
check_cxx_source_compiles("
	#define BOOST_TEST_DYN_LINK
	#define BOOST_TEST_MAIN
	#include <boost/test/unit_test.hpp>
" BOOST_TEST_DYN_LINK)

if(BOOST_TEST_DYN_LINK)
	target_compile_definitions(test_utils PRIVATE BOOST_TEST_DYN_LINK)
endif(BOOST_TEST_DYN_LINK)

