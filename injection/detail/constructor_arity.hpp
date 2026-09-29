#pragma once

#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>


namespace injection::detail {
    inline constexpr std::size_t max_constructor_arity = 16;

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
    consteval bool constructible_with(std::index_sequence<I...>) {
        return std::is_constructible_v<T, repeat<probe_arg<T>, I>...>;
    }

    // Determine the largest arity of the constructors of type T.
    template <class T, std::size_t N = max_constructor_arity>
    consteval std::size_t constructor_arity() {
        if constexpr (constructible_with<T>(std::make_index_sequence<N>{})) {
            return N;
        } else if constexpr (N > 0) {
            return constructor_arity<T, N-1>();
        } else {
            static_assert(false, "No constructor injectable from smart pointer arguments");
            return 0;
        }
    }
}
