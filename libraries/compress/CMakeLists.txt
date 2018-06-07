
add_library(compress
    src/amount.cpp
)

target_link_libraries(compress core)

target_include_directories(compress PUBLIC core include)
