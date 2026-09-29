#include <catch2/catch_test_macros.hpp>

#include "test_types.hpp"

using injection::container;
using injection::container_error;
using namespace test_types;


TEST_CASE("transients", "[transients]") {
    container c;

    SECTION("create returns a new instance each call") {
        c.register_transient<engine>();

        const auto a = c.create<engine>();
        const auto b = c.create<engine>();

        REQUIRE(a != nullptr);
        REQUIRE(b != nullptr);
        REQUIRE(a.get() != b.get());
    }

    SECTION("interface is mapped to its implementation") {
        c.register_transient<shape, square>();

        const auto s = c.create<shape>();

        REQUIRE(s->name() == "square");
        REQUIRE(dynamic_cast<square*>(s.get()) != nullptr);
    }

    SECTION("re-registering replaces the implementation") {
        c.register_transient<shape, square>();
        c.register_transient<shape, circle>();

        REQUIRE(c.create<shape>()->name() == "circle");
    }

    SECTION("unregistered type throws") {
        REQUIRE_THROWS_AS(c.create<shape>(), container_error);
    }
}
