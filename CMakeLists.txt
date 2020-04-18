cmake_minimum_required(VERSION 3.12)

project(yacis)

if (MSVC)
    set(yacis_compile_options /W4)
else ()
    set(yacis_compile_options -Wall -Wextra -pedantic)
endif ()

add_subdirectory(third_party/pegtl)

file(GLOB_RECURSE yacis_sources src/*.cpp)

add_executable(yacis ${yacis_sources})
target_include_directories(yacis PRIVATE include)
target_link_libraries(yacis PRIVATE taocpp::pegtl)
target_compile_features(yacis PRIVATE cxx_std_17)
target_compile_options(yacis PRIVATE ${yacis_compile_options})
