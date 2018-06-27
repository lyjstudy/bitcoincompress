
# helper library
add_library(helper
    src/scan.cpp
    src/block_file.cpp
)

target_link_libraries(helper bkbase core script)
target_include_directories(helper PUBLIC include)

