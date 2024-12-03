#pragma once

#include <type_traits>

namespace aoc::templates {
    template<typename T>
    concept Numeric = std::is_arithmetic_v<T>;

    template<typename F, typename T>
    concept ListBinaryOperation = Numeric<T> && requires(F f, T a, T b)
    {
        { f(a, b) } -> std::convertible_to<T>;
    };

    template<typename F, typename T>
    concept VectorOperation = requires(F f, const std::span<const T> span) {
        { f(span) } -> std::convertible_to<bool>;
    };

}