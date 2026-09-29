#pragma once

#include <memory>
#include <string>

#include "injection/container.hpp"


namespace test_types {
    struct shape {
        virtual ~shape() = default;
        [[nodiscard]] virtual std::string name() const = 0;
    };

    struct square : shape {
        square() = default;
        [[nodiscard]] std::string name() const override;
    };

    struct circle : shape {
        circle() = default;
        [[nodiscard]] std::string name() const override;
    };

    struct config {
        static int instances;

        std::string host;
        int port;

        config(std::string host, int port);
    };

    // Constructor pulls one dependency from each source: transient, singleton and value.
    struct widget {
        std::unique_ptr<shape> sh;
        std::shared_ptr<config> cfg;
        int size;

        widget(std::unique_ptr<shape> sh, std::shared_ptr<config> cfg, int size);
    };

    struct engine {
        engine() = default;
    };

    struct car {
        std::unique_ptr<engine> eng;

        explicit car(std::unique_ptr<engine> eng);
    };

    struct garage {
        std::unique_ptr<car> vehicle;

        explicit garage(std::unique_ptr<car> vehicle);
    };

    struct shapes_registrar {
        static void reg(injection::container& c);
    };
}
