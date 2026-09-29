#include "test_types.hpp"

#include <utility>


namespace test_types {
    std::string square::name() const { return "square"; }

    std::string circle::name() const { return "circle"; }

    int config::instances = 0;

    config::config(std::string host, const int port) : host { std::move(host) }, port { port } {
        ++instances;
    }

    widget::widget(std::unique_ptr<shape> sh, std::shared_ptr<config> cfg, const int size)
        : sh { std::move(sh) }, cfg { std::move(cfg) }, size { size } {}

    car::car(std::unique_ptr<engine> eng) : eng { std::move(eng) } {}

    garage::garage(std::unique_ptr<car> vehicle) : vehicle { std::move(vehicle) } {}

    void shapes_registrar::reg(injection::container& c) {
        c.register_transient<shape, circle>();
        c.register_value(7);
    }
}
