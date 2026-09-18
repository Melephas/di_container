#pragma once

#include <any>
#include <functional>
#include <memory>
#include <typeindex>

#include "container.hpp"


namespace injection {
    class container;

    template <class T>
    concept registerable = requires(T a, container& c) {
        T::reg(c);
    };

    struct container_error : std::runtime_error {
        explicit container_error(const char* m) : std::runtime_error(m) {}
        explicit container_error(const std::string& m) : std::runtime_error(m) {}
    };

    class construction_spec {
    public:
        std::function<std::any(container&)> builder;
    };

    class container {
        std::unordered_map<std::type_index, std::any> services;
        std::unordered_map<std::type_index, construction_spec> transients;
    public:
        virtual ~container() = default;

        template <class ServiceType, class ImplementationType, class... Args>
        void register_singleton(Args&&... args) {
            std::shared_ptr<ServiceType> impl_ptr = std::make_shared<ImplementationType>(std::forward<Args>(args)...);
            services.insert_or_assign(typeid(ServiceType), impl_ptr);
        }

        template <class ServiceType, class ImplementationType, class... Args>
        void register_transient(/*Args&&... args*/) {
            construction_spec c {
                [](container& container) -> std::any {
                    std::shared_ptr<ServiceType> impl = std::make_shared<ImplementationType>(container.resolve<Args>()...);
                    return impl;
                }
            };
            transients.insert_or_assign(typeid(ServiceType), c);
        }

        template <registerable R>
        void reg() {
            R::reg(*this);
        }

        template <class T>
        std::shared_ptr<T> resolve() {
            std::type_index const tid = typeid(T);
            if (services.contains(tid)) {
                auto impl = services.at(typeid(T));
                return std::any_cast<std::shared_ptr<T>>(impl);
            }

            if (transients.contains(tid)) {
                auto impl = transients.at(tid).builder(*this);
                return std::any_cast<std::shared_ptr<T>>(impl);
            }

            std::stringstream ss {};
            ss << "unknown type index: " << tid.name();
            throw container_error { ss.str() };
        }

        // template <class >
    };
}
