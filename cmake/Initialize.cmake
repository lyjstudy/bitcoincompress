
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

# Install boost
# curl -LO https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.bz2
# tar xf boost_1_67_0.tar.bz2 && cd boost_1_67_0
# ./bootstrap.sh --prefix=/opt/boost_1_67_0
# sudo ./b2 install
set(BOOST_ROOT "/opt/boost_1_67_0")
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS OFF)
set(BOOST_ALL_DYN_LINK ON)
add_definitions(-DBOOST_ALL_DYN_LINK=1)

find_package(Boost 1.67 REQUIRED COMPONENTS
	thread
	system
    date_time
    filesystem
	log_setup
	log
    unit_test_framework
)

set(CORE_LIBRARIES pthread ${OPENSSL_CRYPTO_LIBRARY} ${SECP256K1_LIBRARY} ${LEVELDB_LIBRARY} ${Boost_LIBRARIES})
set(CORE_INCLUDES ${OPENSSL_CRYPTO_INCLUDES} ${SECP256K1_INCLUDES} ${LEVELDB_INCLUDES} ${Boost_INCLUDE_DIRS})

set(BKBASE_LIBRARIES ${CORE_LIBRARIES})
set(BKBASE_INCLUDES ${CORE_INCLUDES})

find_program(CCACHE ccache)
if(CCACHE)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE})
endif(CCACHE)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

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