#include "container_error.hpp"


namespace injection {
    container_error::container_error(const char* m) : std::runtime_error(m) {}

    container_error::container_error(const std::string& m) : std::runtime_error(m) {}
}
