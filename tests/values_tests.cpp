#include <string>

#include <catch2/catch_test_macros.hpp>

#include "test_types.hpp"

using injection::container;
using injection::container_error;
using namespace test_types;


TEST_CASE("values", "[values]") {
    container c;

    SECTION("registered value is resolved") {
        c.register_value(42);
        c.register_value(std::string { "hello" });

        REQUIRE(c.resolve_value<int>() == 42);
        REQUIRE(c.resolve_value<std::string>() == "hello");
    }

    SECTION("re-registering overwrites the value") {
        c.register_value(1);
        c.register_value(2);

        REQUIRE(c.resolve_value<int>() == 2);
    }

    SECTION("unregistered value throws") {
        REQUIRE_THROWS_AS(c.resolve_value<int>(), container_error);
    }
}
