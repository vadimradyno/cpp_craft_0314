#include "test_registrator.h"

#include <boost/test/unit_test.hpp>

boost::unit_test_framework::test_suite* init_unit_test_suite( int , char* [] )
{
	using boost::unit_test_framework::test_case;

    boost::unit_test_framework::test_suite* ts1 = BOOST_TEST_SUITE( "task 5_4 tests" );
	boost::unit_test::unit_test_log.set_threshold_level( boost::unit_test::log_messages );

	using namespace task5_4::tests_;

	ts1->add( BOOST_TEST_CASE( &template_functions_vector_tests ) );	
	ts1->add( BOOST_TEST_CASE( &template_functions_map_tests ) );	
	ts1->add( BOOST_TEST_CASE( &template_functions_list_tests ) );	
	ts1->add( BOOST_TEST_CASE( &template_functions_set_tests ) );	

#ifdef RUN_PERFORMANCE_TESTS
#endif

	return ts1;
}
