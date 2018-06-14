

add_executable(blockscan
    src/main.cpp
    src/block_file.cpp
)


target_link_libraries(blockscan bkbase core)

target_include_directories(blockscan PUBLIC include)
