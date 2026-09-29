cmake_minimum_required(VERSION 4.1)
project(di_container CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        Logging
        GIT_REPOSITORY https://github.com/Melephas/Logging-Cpp.git
        GIT_TAG        e8e328282265f88e7f7303d1f30835f445a8652f
)

FetchContent_MakeAvailable(Logging)


add_library(
    injection STATIC
        injection/container.cpp
        injection/container_error.cpp
)

target_include_directories(injection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(injection PRIVATE
        -Wall -Wextra -Wpedantic -Werror
)

add_executable(
    di_container
        main.cpp
)

target_compile_options(di_container PRIVATE
        -Wall -Wextra -Wpedantic -Werror
)

target_link_libraries(di_container PRIVATE injection Logging)

option(DI_CONTAINER_BUILD_TESTS "Build unit tests" ON)
if (DI_CONTAINER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
