cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0076 NEW)

project(
    ip_filter_lib
)

add_library(lib INTERFACE)
target_sources(lib INTERFACE ip_filter.hpp)
target_include_directories(lib INTERFACE "${CMAKE_SOURCE_DIR}/lib")

set_property(
    TARGET lib
        APPEND PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES
                "${CMAKE_BINARY_DIR}"
                "${CMAKE_SOURCE_DIR}/lib"
)

install(
    TARGETS lib
        LIBRARY DESTINATION lib
            COMPONENT lib
)
