#include "container.hpp"

#include <format>


namespace injection {
    container::~container() = default;

    void container::throw_not_unique(const std::type_index tid) {
        throw container_error {
            std::format("'{}' is a singleton and cannot be uniquely owned", tid.name())
        };
    }

    void container::throw_unknown_type(const std::type_index tid) {
        throw container_error {
            std::format("unknown type index: '{}'", tid.name())
        };
    }

    void container::throw_no_value(const std::type_index tid) {
        throw container_error { std::format("no value of type {} registered", tid.name()) };
    }
}
