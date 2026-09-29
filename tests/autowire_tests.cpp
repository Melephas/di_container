#include <catch2/catch_test_macros.hpp>

#include "test_types.hpp"

using injection::container;
using injection::container_error;
using namespace test_types;


TEST_CASE("auto-wiring", "[autowire]") {
    container c;

    SECTION("dependencies come from the matching source") {
        c.register_transient<shape, square>();
        c.register_singleton<config, config>("localhost", 8080);
        c.register_value(3);
        c.register_transient<widget>();

        const auto w = c.create<widget>();

        REQUIRE(w->sh->name() == "square");
        REQUIRE(w->cfg == c.resolve<config>());
        REQUIRE(w->size == 3);
    }

    SECTION("nested dependency chains resolve") {
        c.register_transient<engine>();
        c.register_transient<car>();
        c.register_transient<garage>();

        const auto g = c.create<garage>();

        REQUIRE(g->vehicle != nullptr);
        REQUIRE(g->vehicle->eng != nullptr);
    }

    SECTION("missing dependency throws") {
        c.register_singleton<config, config>("localhost", 8080);
        c.register_value(3);
        c.register_transient<widget>();

        REQUIRE_THROWS_AS(c.create<widget>(), container_error);
    }

    SECTION("missing value dependency throws") {
        c.register_transient<shape, square>();
        c.register_singleton<config, config>("localhost", 8080);
        c.register_transient<widget>();

        REQUIRE_THROWS_AS(c.create<widget>(), container_error);
    }
}
