#include <iostream>
#include <print>
#include <sstream>
#include <string>

#include "logging.hpp"
#include "injection/container.hpp"

//
// struct service {
//     virtual ~service() = default;
//     virtual std::string get_name() = 0;
// };
//
// struct service_impl : virtual service {
//     ~service_impl() override = default;
//     std::string get_name() override {
//         return "service_impl";
//     }
//
//     static void reg(injection::container& c) {
//         c.register_singleton<service, service_impl>();
//     }
// };
//
// struct decorator {
//     virtual ~decorator() = default;
//     virtual std::string decorate() = 0;
// };
//
// class decorator_impl : virtual public decorator {
//     std::shared_ptr<service> svc;
//
// public:
//     explicit decorator_impl(std::shared_ptr<service> svc) : svc(std::move(svc)) {}
//     ~decorator_impl() override = default;
//
//     std::string decorate() override {
//         std::stringstream ss {};
//         ss << "*" << svc->get_name() << "*";
//         return ss.str();
//     }
//
//     static void reg(injection::container& c) {
//         c.register_transient<decorator, decorator_impl, service>();
//     }
// };

struct register_logging {
    static void reg(injection::container& c) {
        logging::logger::simple_logger();
        c.register_transient<
            logging::logger::logger, logging::logger::simple_logger,
            logging::level,
        >();
    }
};

int main() {
    injection::container container {};

    container.reg<register_logging>();

    const auto svc = container.resolve<logging::logger::logger>();

    svc->info("Hello, World!");

    return 0;
}
