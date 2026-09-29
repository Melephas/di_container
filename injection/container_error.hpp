#pragma once

#include <stdexcept>
#include <string>


namespace injection {
    struct container_error : std::runtime_error {
        explicit container_error(const char* m);
        explicit container_error(const std::string& m);
    };
}
