#include <catch2/catch_test_macros.hpp>

#include "test_types.hpp"

using injection::container;
using namespace test_types;


TEST_CASE("registerable concept", "[concepts]") {
    STATIC_REQUIRE(injection::registerable<shapes_registrar>);
    STATIC_REQUIRE_FALSE(injection::registerable<engine>);
}


TEST_CASE("reg applies a registrar", "[registrar]") {
    container c;
    c.reg<shapes_registrar>();

    REQUIRE(c.create<shape>()->name() == "circle");
    REQUIRE(c.resolve_value<int>() == 7);
}
