cmake_minimum_required(VERSION 4.1)
project(di_container CXX)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        Logging
        GIT_REPOSITORY https://github.com/Melephas/Logging-Cpp.git
        GIT_TAG        29932da29bad053bfd77d1a033ffec34e08c7f9b
)

FetchContent_MakeAvailable(Logging)


add_executable(
    di_container
        main.cpp
        injection/container.hpp
)

target_compile_options(di_container PRIVATE
        -Wall -Wextra -Wpedantic -Werror
)

target_link_libraries(di_container PRIVATE Logging)
