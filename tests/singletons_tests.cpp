#include <catch2/catch_test_macros.hpp>

#include "test_types.hpp"

using injection::container;
using injection::container_error;
using namespace test_types;


TEST_CASE("singletons", "[singletons]") {
    container c;

    SECTION("constructor arguments are forwarded") {
        c.register_singleton<config, config>("localhost", 8080);

        const auto cfg = c.resolve<config>();

        REQUIRE(cfg->host == "localhost");
        REQUIRE(cfg->port == 8080);
    }

    SECTION("resolve returns the same instance each call") {
        config::instances = 0;
        c.register_singleton<config, config>("localhost", 8080);

        const auto a = c.resolve<config>();
        const auto b = c.resolve<config>();

        REQUIRE(a == b);
        REQUIRE(config::instances == 1);
    }

    SECTION("interface is mapped to its implementation") {
        c.register_singleton<shape, circle>();

        REQUIRE(c.resolve<shape>()->name() == "circle");
    }

    SECTION("create on a singleton throws") {
        c.register_singleton<config, config>("localhost", 8080);

        REQUIRE_THROWS_AS(c.create<config>(), container_error);
    }
}
