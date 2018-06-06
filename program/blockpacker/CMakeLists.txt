

add_executable(blockpacker
    src/main.cpp
    src/block_pool.cpp
    src/compress.cpp
)


target_link_libraries(blockpacker core utils script)

target_include_directories(blockpacker PUBLIC include)
