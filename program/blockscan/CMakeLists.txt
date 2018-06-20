

add_executable(blockscan
    src/main.cpp
)


target_link_libraries(blockscan bkbase core script helper)

target_include_directories(blockscan PUBLIC include)
