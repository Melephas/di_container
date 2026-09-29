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
        template <class T>
        using factory = std::function<std::unique_ptr<T>(container&)>;

        std::unordered_map<std::type_index, std::any> values;
        std::unordered_map<std::type_index, std::any> singletons;
        std::unordered_map<std::type_index, std::any> transients;

        static constexpr std::size_t max_constructor_arity = 16;

        // Allows compile time resolution of constructor args.
        template <class O> struct probe_arg {
            // ReSharper disable once CppFunctionIsNotImplemented
            // ReSharper disable once CppNonExplicitConversionOperator
            template <class U> operator std::shared_ptr<U>() const;

            // ReSharper disable once CppFunctionIsNotImplemented
            // ReSharper disable once CppNonExplicitConversionOperator
            template <class U> operator std::unique_ptr<U>() const;

            // ReSharper disable once CppFunctionIsNotImplemented
            // ReSharper disable once CppNonExplicitConversionOperator
            template <class U> requires (!std::same_as<std::remove_cvref_t<U>, O>)
            operator U() const;
        };

        template <class Arg, std::size_t> using repeat = Arg;

        // Determine if a particular class has a constructor with I arguments.
        template <class T, std::size_t... I>
        static consteval bool constructible_with(std::index_sequence<I...>) {
            return std::is_constructible_v<T, repeat<probe_arg<T>, I>...>;
        }

        // Determine the largest arity of the constructors of type T.
        template <class T, std::size_t N = max_constructor_arity>
        static consteval std::size_t constructor_arity() {
            if constexpr (constructible_with<T>(std::make_index_sequence<N>{})) {
                return N;
            } else if constexpr (N > 0) {
                return constructor_arity<T, N-1>();
            } else {
                static_assert(false, "No constructor injectable from smart pointer arguments");
                return 0;
            }
        }

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
            return std::make_unique<T>(repeat<auto_arg<T>, I>{ *this }...);
        }

    public:
        virtual ~container() = default;

        template <class ServiceType, class ImplementationType, class... Args>
        void register_singleton(Args&&... args) {
            std::shared_ptr<ServiceType> impl_ptr = std::make_shared<ImplementationType>(std::forward<Args>(args)...);
            singletons.insert_or_assign(typeid(ServiceType), impl_ptr);
        }

        template <class ServiceType, class ImplementationType = ServiceType>
        void register_transient() {
            static_assert(
                std::same_as<ServiceType, ImplementationType> || std::has_virtual_destructor_v<ServiceType>,
                "std::unique_ptr<ServiceType> needs a virtual destructor to delete a derived type"
            );

            const std::type_index service_tid = typeid(ServiceType);

            constexpr std::size_t arity = constructor_arity<ImplementationType>();

            transients.insert_or_assign(service_tid, factory<ServiceType> {
                [](container& c) -> std::unique_ptr<ServiceType> {
                    return c.make_auto<ImplementationType>(std::make_index_sequence<arity>{});
                }
            });
        }

        template <class Type>
        void register_value(Type val) {
            values.insert_or_assign(typeid(Type), val);
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
                throw container_error {
                    std::format("'{}' is a singleton and cannot be uniquely owned", tid.name())
                };
            }

            throw container_error {
                std::format("unknown type index: '{}'", tid.name())
            };
        }

        template <class T>
        std::shared_ptr<T> resolve() {
            const std::type_index tid = typeid(T);
            if (const auto iter = singletons.find(tid); iter != singletons.cend()) {
                return std::any_cast<std::shared_ptr<T>>(iter->second);
            }

            return create<T>();
        }

        template <class Type>
        Type resolve_value() {
            if (values.contains(typeid(Type))) {
                decltype(auto) val = values.at(typeid(Type));
                return std::any_cast<Type>(val);
            }

            throw container_error { std::format("no value of type {} registered", typeid(Type).name()) };
        }
    };
}
