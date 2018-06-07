

add_executable(blockscan
    src/main.cpp
)


target_link_libraries(blockscan core utils script)

target_include_directories(blockscan PUBLIC include)
