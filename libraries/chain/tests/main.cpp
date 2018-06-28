#include <boost/test/unit_test.hpp>

#ifdef BOOST_TEST_ALTERNATIVE_INIT_API
bool        init_unit_test() {
#else
::boost::unit_test::test_suite* init_unit_test( int, char* [] ) {
#endif

#ifdef BOOST_TEST_ALTERNATIVE_INIT_API
    return true;
}
#else
    return nullptr;
}
#endif

int BOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
    return ::boost::unit_test::unit_test_main( init_unit_test, argc, argv );
}
