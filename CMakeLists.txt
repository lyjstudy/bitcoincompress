cmake_minimum_required(VERSION 3.5)

project(BitcoinKeys)

set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(FindBoost)
include(AddCompilerFlags)
include(TestSuite)
include(Initialize)

option(BITCOINKEYS_BUILD_TESTS "Build BitcoinKeys's unit tests" ON)


add_subdirectory(libraries)
add_subdirectory(program)

