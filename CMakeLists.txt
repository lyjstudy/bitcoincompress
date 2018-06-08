cmake_minimum_required(VERSION 3.5)

project(BitcoinKeys)

option(BITCOINKEYS_BUILD_TESTS "Build BitcoinKeys's unit tests" ON)

# Install openssl to /opt/openssl
# git clone -b OpenSSL_1_1_0h https://github.com/openssl/openssl.git
# cd openssl && mkdir build && cd build
# ../config --prefix=/opt/openssl
# make -j8 && sudo make install
set(OPENSSL_CRYPTO_LIBRARY "/opt/openssl/lib/libcrypto.so")
set(OPENSSL_CRYPTO_INCLUDES "/opt/openssl/include")

# Install secp256k1 to /opt/secp256k1
# git clone https://github.com/lyjstudy/secp256k1.git
# cd secp256k1
# ./autogen.sh
# cd build
# ../configure --prefix=/opt/secp256k1
# make -j8 && sudo make install
set(SECP256K1_LIBRARY "/opt/secp256k1/lib/libsecp256k1.so")
set(SECP256K1_INCLUDES "/opt/secp256k1/include")

# Install leveldb to /opt/leveldb
# git clone https://github.com/lyjstudy/leveldb.git
# cd leveldb && mkdir build && cd build
# cmake .. -DCMAKE_INSTALL_PREFIX=/opt/leveldb -DBUILD_SHARED_LIBS
# make -j8 && sudo make install
set(LEVELDB_LIBRARY "/opt/leveldb/lib/libleveldb.so")
set(LEVELDB_INCLUDES "/opt/leveldb/include")

# boost filesystem
find_package(Boost 1.58 REQUIRED filesystem)

set(CORE_LIBRARIES ${OPENSSL_CRYPTO_LIBRARY} ${SECP256K1_LIBRARY} ${LEVELDB_LIBRARY} Boost::filesystem)
set(CORE_INCLUDES ${OPENSSL_CRYPTO_INCLUDES} ${SECP256K1_INCLUDES} ${LEVELDB_INCLUDES} Boost::filesystem)

# unit test libraries
if (BITCOINKEYS_BUILD_TESTS)
	find_package(Boost 1.58 REQUIRED unit_test_framework)
endif (BITCOINKEYS_BUILD_TESTS)


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
	-pthread
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

