cmake_minimum_required(VERSION 3.27)

project(hexpander
        VERSION 0.0.1
        LANGUAGES CXX)
set(PROJECT_DESCRIPTION "Expanding strings to anything you define")
set(CMAKE_SYSTEM_VERSION 10.0.22631)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib)

add_library(${PROJECT_NAME}_lib
    src/cmd_resolver.cpp
    src/cmd_resolver.h
    src/expander.cpp
    src/expander.h
    src/expander_feeder_intf.h
    src/expander_intf.h
    src/expander_recursive_intf.h
    src/expander_resolver_intf.h
    src/light_expander.cpp
    src/light_expander.h
    src/registry_intf.h
    src/registry_resolver.cpp
    src/registry_resolver.h
    src/simple_resolver.cpp
    src/simple_resolver.h
    src/time_measure.h
    src/utils.h
)

#target_include_directories(${PROJECT_NAME}_lib
    #include/
#)

target_compile_features("${PROJECT_NAME}_lib" PRIVATE cxx_std_20)

# UnitTests
add_library(${PROJECT_NAME}_tests SHARED
            tests/tests.cpp
            tests/fake.h)

target_link_libraries(${PROJECT_NAME}_tests 
                      ${PROJECT_NAME}_lib)

target_compile_features("${PROJECT_NAME}_tests" PRIVATE cxx_std_20)

# Set build configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release)

