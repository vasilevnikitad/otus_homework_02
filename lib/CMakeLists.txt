cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()

project(
    ip_filter_lib
)


add_library(lib INTERFACE)
if(POLICY CMP0076)
    target_sources(lib INTERFACE ip_filter.hpp)
else()
    target_sources(lib
                    INTERFACE "${CMAKE_SOURCE_DIR}/lib/ip_filter.hpp")
endif()

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
