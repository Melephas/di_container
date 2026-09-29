#include <source_location>

#include "logging.hpp"
#include "injection/container.hpp"

auto fname(const std::source_location s = std::source_location::current()) {
    return s.function_name();
}

struct service {
    virtual ~service() = default;
    virtual std::string get_message() = 0;
};

struct implementation : virtual service {
    ~implementation() override = default;

    std::string get_message() override {
        return "Hello, World!";
    }
};


struct register_logging {
    [[maybe_unused]] static void reg(injection::container& c) {
        c.register_value(logging::level::debug);
        c.register_value(std::vector<logging::filter::filter>());
        c.register_transient<logging::format::formatter, logging::format::simple_formatter>();
        c.register_transient<logging::handling::handler, logging::handling::stdout_handler>();
        c.register_transient<logging::logger::logger, logging::logger::simple_logger>();
    }
};


class program {
    std::unique_ptr<logging::logger::logger> log;
    std::unique_ptr<service> message_service;

public:
    program(std::unique_ptr<logging::logger::logger> log, std::unique_ptr<service> svc)
        : log { std::move(log) }, message_service { std::move(svc) } {}

    void run() const {
        log->debug(std::format("Entering function {}", fname()));

        const auto message = message_service->get_message();
        log->info(message);
    }
};


int main() {
    injection::container container {};

    container.reg<register_logging>();
    container.register_transient<service, implementation>();
    container.register_transient<program>();

    const auto prog = container.create<program>();
    prog->run();

    return 0;
}
