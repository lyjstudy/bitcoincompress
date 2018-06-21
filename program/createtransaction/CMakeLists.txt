

add_executable(createtransaction
    src/main.cpp
)


target_link_libraries(createtransaction core script)

target_include_directories(createtransaction PUBLIC include)
