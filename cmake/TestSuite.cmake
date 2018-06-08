# Allow to easily build test suites

macro(create_test_suite NAME)
	enable_testing()
	set(TARGET "check-${NAME}")
	add_custom_target(${TARGET} COMMAND ${CMAKE_CTEST_COMMAND})

	# If the magic target check-all exists, attach to it.
	if(TARGET check-all)
		add_dependencies(check-all ${TARGET})
	endif()
endmacro(create_test_suite)

function(add_test_to_suite SUITE NAME)
	add_executable(${NAME} EXCLUDE_FROM_ALL ${ARGN})
	add_test(${NAME} ${NAME})
	add_dependencies("check-${SUITE}" ${NAME})
endfunction(add_test_to_suite)

function(bitcoinkeys_test_suite NAME)
	create_test_suite(${NAME})
	add_test_to_suite(${NAME} "test_${NAME}" EXCLUDE_FROM_ALL ${ARGN})
	target_include_directories("test_${NAME}" PUBLIC include)
	target_link_libraries("test_${NAME}" Boost::unit_test_framework ${NAME})
	target_compile_definitions("test_${NAME}" PRIVATE BOOST_TEST_DYN_LINK)
endfunction(bitcoinkeys_test_suite NAME)