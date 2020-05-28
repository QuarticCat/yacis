cmake_minimum_required(VERSION 3.12)

project(yacis)

option(PEGTL_BUILD_TESTS OFF)
option(PEGTL_BUILD_EXAMPLES OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/pegtl)

option(AS_EXECUTABLE OFF)

if (AS_EXECUTABLE)
    if (MSVC)
        set(yacis_compile_options /W4)
    else ()
        set(yacis_compile_options -Wall -Wextra -pedantic)
    endif ()

    file(GLOB_RECURSE yacis_sources ${PROJECT_SOURCE_DIR}/include/*.hpp ${PROJECT_SOURCE_DIR}/src/main.cpp)

    add_executable(yacis ${yacis_sources})
    target_include_directories(yacis PRIVATE ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(yacis PRIVATE taocpp::pegtl)
    target_compile_features(yacis PRIVATE cxx_std_17)
    target_compile_options(yacis PRIVATE ${yacis_compile_options})
else ()
    add_library(yacis INTERFACE)
    target_include_directories(yacis INTERFACE ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(yacis INTERFACE taocpp::pegtl)
    target_compile_features(yacis INTERFACE cxx_std_17)
endif ()

