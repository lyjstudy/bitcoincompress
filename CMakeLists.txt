cmake_minimum_required(VERSION 3.5)

project(BitcoinKeys)

# Install openssl to /opt/openssl
# git clone -b OpenSSL_1_1_0h https://github.com/openssl/openssl.git
# cd openssl && mkdir build && cd build
# ../config --prefix=/opt/openssl
# make -j8 && sudo make install
set(OPENSSL_CRYPTO_LIBRARY "/opt/openssl/lib/libcrypto.so")
set(OPENSSL_CRYPTO_INCLUDES "/opt/openssl/include")

set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

find_program(CCACHE ccache)
if(CCACHE)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE})
endif(CCACHE)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

include(AddCompilerFlags)
include(TestSuite)

add_c_compiler_flag(-Wnested-externs -Wstrict-prototypes)
add_compiler_flag(
	-Wall
	-Wextra
	-Wformat
	-Wvla
	-Wformat-security
	-Wcast-align
	-Wunused-parameter
)

option(EXTRA_WARNINGS "Enable extra warnings" OFF)
if(EXTRA_WARNINGS)
	add_compiler_flag(-Wshadow)
	add_cxx_compiler_flag(-Wsuggest-override)
else()
	add_compiler_flag(-Wno-unused-parameter)
endif()


add_subdirectory(libraries)
add_subdirectory(program)
# add_subdirectory(tests)

