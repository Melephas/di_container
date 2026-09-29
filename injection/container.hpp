#pragma once

#include <any>
#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "container_error.hpp"
#include "registerable.hpp"
#include "detail/constructor_arity.hpp"


namespace injection {
    class container {
        template <class T>
        using factory = std::function<std::unique_ptr<T>(container&)>;

        std::unordered_map<std::type_index, std::any> values;
        std::unordered_map<std::type_index, std::any> singletons;
        std::unordered_map<std::type_index, std::any> transients;

        template <class O> struct auto_arg {
            container& c;

            // ReSharper disable once CppNonExplicitConversionOperator
            template <class U> operator std::shared_ptr<U>() const {
                return c.resolve<std::remove_cv_t<U>>();
            }

            // ReSharper disable once CppNonExplicitConversionOperator
            template <class U> operator std::unique_ptr<U>() const {
                return c.create<std::remove_cv_t<U>>();
            }

            template <class U> requires (!std::same_as<std::remove_cvref_t<U>, O>)
            operator U() const {
                return c.resolve_value<U>();
            }
        };

        template <class T, std::size_t... I>
        std::unique_ptr<T> make_auto(std::index_sequence<I...>) {
            return std::make_unique<T>(detail::repeat<auto_arg<T>, I>{ *this }...);
        }

        [[noreturn]] static void throw_not_unique(std::type_index tid);
        [[noreturn]] static void throw_unknown_type(std::type_index tid);
        [[noreturn]] static void throw_no_value(std::type_index tid);

    public:
        virtual ~container();

        template <class ServiceType, class ImplementationType, class... Args>
        void register_singleton(Args&&... args) {
            std::shared_ptr<ServiceType> impl_ptr = std::make_shared<ImplementationType>(std::forward<Args>(args)...);
            singletons.insert_or_assign(typeid(ServiceType), std::move(impl_ptr));
        }

        template <class ServiceType, class ImplementationType = ServiceType>
        void register_transient() {
            static_assert(
                std::same_as<ServiceType, ImplementationType> || std::has_virtual_destructor_v<ServiceType>,
                "std::unique_ptr<ServiceType> needs a virtual destructor to delete a derived type"
            );

            constexpr std::size_t arity = detail::constructor_arity<ImplementationType>();

            transients.insert_or_assign(typeid(ServiceType), factory<ServiceType> {
                [](container& c) -> std::unique_ptr<ServiceType> {
                    return c.make_auto<ImplementationType>(std::make_index_sequence<arity>{});
                }
            });
        }

        template <class Type>
        void register_value(Type val) {
            values.insert_or_assign(typeid(Type), std::move(val));
        }

        template <registerable R>
        void reg() {
            R::reg(*this);
        }

        template <class T>
        std::unique_ptr<T> create() {
            const std::type_index tid = typeid(T);

            if (const auto iter = transients.find(tid); iter != transients.cend()) {
                return std::any_cast<factory<T>&>(iter->second)(*this);
            }

            if (singletons.contains(tid)) {
                throw_not_unique(tid);
            }

            throw_unknown_type(tid);
        }

        template <class T>
        std::shared_ptr<T> resolve() {
            if (const auto iter = singletons.find(typeid(T)); iter != singletons.cend()) {
                return std::any_cast<std::shared_ptr<T>>(iter->second);
            }

            return create<T>();
        }

        template <class Type>
        Type resolve_value() {
            if (const std::type_index tid = typeid(Type); values.contains(tid)) {
                decltype(auto) val = values.at(tid);
                return std::any_cast<Type>(val);
            }

            throw_no_value(typeid(Type));
        }
    };
}
