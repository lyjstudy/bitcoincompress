add_custom_target(check)
enable_testing()

function(bitcoinkeys_test_suite NAME)
	set(TARGET "check-${NAME}")
	set(EXECUTABLE "test_${NAME}")

	add_custom_target(${TARGET} COMMAND ${CMAKE_CTEST_COMMAND})
	add_dependencies(check ${TARGET})

	add_executable(${EXECUTABLE} EXCLUDE_FROM_ALL ${ARGN})
	add_test(${EXECUTABLE} ${EXECUTABLE})
	add_dependencies(${TARGET} ${EXECUTABLE})

	target_include_directories(${EXECUTABLE} PUBLIC include)
	target_link_libraries(${EXECUTABLE} ${NAME})
	cotire(${EXECUTABLE})
endfunction(bitcoinkeys_test_suite NAME)
