
add_library(core
    src/serialize.cpp
    src/hash.cpp
    src/hex.cpp
    src/transaction.cpp
)


target_link_libraries(core ${OPENSSL_CRYPTO_LIBRARY})

target_include_directories(core PUBLIC ${OPENSSL_CRYPTO_INCLUDES} include)

create_test_suite(core)

add_test_to_suite(core test_core
    tests/serialize_tests.cpp
    tests/hash_tests.cpp
    tests/hex_tests.cpp
    tests/transaction_tests.cpp
)

target_include_directories(test_core
	PUBLIC include
)

find_package(Boost 1.58 REQUIRED unit_test_framework)
target_link_libraries(test_core Boost::unit_test_framework core)

# We need to detect if the BOOST_TEST_DYN_LINK flag is required.
set(CMAKE_REQUIRED_LIBRARIES Boost::unit_test_framework)
check_cxx_source_compiles("
	#define BOOST_TEST_DYN_LINK
	#define BOOST_TEST_MAIN
	#include <boost/test/unit_test.hpp>
" BOOST_TEST_DYN_LINK)

if(BOOST_TEST_DYN_LINK)
	target_compile_definitions(test_core PRIVATE BOOST_TEST_DYN_LINK)
endif(BOOST_TEST_DYN_LINK)

