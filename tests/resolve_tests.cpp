#include <catch2/catch_test_macros.hpp>

#include "test_types.hpp"

using injection::container;
using namespace test_types;


TEST_CASE("resolve falls back to transients", "[resolve]") {
    container c;
    c.register_transient<shape, square>();

    const auto a = c.resolve<shape>();
    const auto b = c.resolve<shape>();

    REQUIRE(a->name() == "square");
    REQUIRE(a != b);
}
