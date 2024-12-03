#pragma once

#include <expected>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <print>
#include <string_view>
#include <regex>
#include <vector>

#include "AocExceptions.h"
#include "AocTemplates.h"

class Day03 {
public:
    Day03() = delete; // This class is not meant to be instantiated
    ~Day03() = delete; // No inheritance either

    static void partOne();

    static void partTwo();

#ifdef TESTING
    friend class Day03Test;
#endif

private:
    using regexIterator = std::sregex_iterator;
    using matchResults = std::smatch;

    static inline const auto mulPattern = std::regex(R"(mul\((\d{1,3}),(\d{1,3})\))", std::regex::optimize);
    static inline const auto sectionPattern = std::regex(R"((do\(\)|^)([\s\S]*?)(don't\(\)|$))", std::regex::optimize);

    template<aoc::templates::Numeric T>
    static T stoT(std::string_view str) noexcept(false);

    template <aoc::templates::Numeric T>
    static auto processMultiplications(std::string_view text);

    template <aoc::templates::Numeric T>
    static T sumMultiplicationsDD(const std::string& input) noexcept;

    static inline std::expected<std::ifstream, aoc::exceptions::AocException> openFile(
    const std::filesystem::path &path) noexcept;

    static inline std::filesystem::path INPUT_FILE{std::filesystem::path{"../data"} / "d3p1.txt"};
};

inline void Day03::partOne() {
    auto file = openFile(INPUT_FILE);
    if (!file) {
        std::println("Error opening file: {}", file.error().what());
        return;
    }

    std::stringstream buffer;
    buffer << file.value().rdbuf();
    const std::string input = buffer.str();

    const auto result = processMultiplications<int64_t>(input);
    std::println("Sum of multiplications: {}", result);
}

inline void Day03::partTwo() {
    auto file = openFile(INPUT_FILE);
    if (!file) {
        std::println("Error opening file: {}", file.error().what());
        return;
    }

    std::stringstream buffer;
    buffer << file.value().rdbuf();
    const std::string input = buffer.str();

    const auto result = sumMultiplicationsDD<int64_t>(input);
    std::println("Sum of multiplications: {}", result);
}

template <aoc::templates::Numeric T>
T Day03::stoT(const std::string_view str) noexcept(false) {
    T value;
    auto *ptr = str.data();
    auto [p, ec] = std::from_chars(ptr, ptr + str.size(), value);
    if (ec != std::errc{}) {
        throw aoc::exceptions::DataFormatError("Invalid number format");
    }
    return value;
}

template <aoc::templates::Numeric T>
auto Day03::processMultiplications(const std::string_view text) {
    const std::string str{text};
    const auto end = regexIterator();

    auto processMatch = [](const T sum, const matchResults& match) {
        const T x = stoT<T>(match[1].str());
        const T y = stoT<T>(match[2].str());
        return sum + x * y;
    };

    return std::accumulate(
        regexIterator(str.begin(), str.end(), mulPattern),
        end,
        T{0},
        processMatch
    );
}

template <aoc::templates::Numeric T>
T Day03::sumMultiplicationsDD(const std::string& input) noexcept {
    auto sections = std::ranges::subrange(  // Gather all sections that match the pattern in a vector
            regexIterator(input.begin(), input.end(), sectionPattern),
            regexIterator()
        ) | std::views::transform([](const auto& match) {
            return match[2].str();
        }) | std::ranges::to<std::vector>();
    return std::transform_reduce(
        std::execution::par_unseq, // ~3800μs to ~3100μs (Debug mode) ~18% faster, ~5% faster (Release mode)
        sections.begin(), sections.end(),
        T{0},
        std::plus{},
        processMultiplications<T>
    );
}

inline std::expected<std::ifstream, aoc::exceptions::AocException> Day03::openFile(
    const std::filesystem::path &path) noexcept {
    if (!exists(path)) {
        return std::unexpected(aoc::exceptions::FileOpenError(path.string()));
    }
    std::ifstream f(path);
    if (!f.is_open()) {
        return std::unexpected(aoc::exceptions::FileOpenError(path.string()));
    }
    return f;
}
