FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.16.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(
    container_tests
        test_types.cpp
        values_tests.cpp
        transients_tests.cpp
        singletons_tests.cpp
        resolve_tests.cpp
        autowire_tests.cpp
        registrar_tests.cpp
)

target_compile_options(container_tests PRIVATE
        -Wall -Wextra -Wpedantic -Werror
)

target_link_libraries(container_tests PRIVATE injection Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(container_tests)
