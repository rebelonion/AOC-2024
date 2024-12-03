#pragma once

#include <expected>
#include <execution>
#include <filesystem>
#include <fstream>
#include <print>
#include <ranges>
#include <vector>

#include <boost/container/small_vector.hpp>

#include "AocExceptions.h"
#include "AocTemplates.h"

class Day02 {
public:
    Day02() = delete; // This class is not meant to be instantiated
    ~Day02() = delete; // No inheritance either

    static void partOne();

    static void partTwoBruteForce();

    static void partTwoSmart();

#ifdef TESTING
    friend class Day02Test;
#endif

private:
    template<aoc::templates::Numeric T, aoc::templates::VectorOperation<T> Op>
    static size_t countSafeParallel(const std::vector<std::vector<T> > &lines, Op op);

    template<aoc::templates::Numeric T>
    [[nodiscard]] static bool rotateAndCheckSafety(std::vector<T> &testVec, std::span<const T> list, size_t i);

    template<aoc::templates::Numeric T>
    [[nodiscard]] static bool isSafeWithChance(std::span<const T> list);

    template<aoc::templates::Numeric T>
    [[nodiscard]] static bool canBeMadeSafe(std::span<const T> list);

    static std::expected<std::ifstream, aoc::exceptions::AocException> openFile(
        const std::filesystem::path &path) noexcept;

    template<aoc::templates::Numeric T>
    static std::expected<std::vector<std::vector<T> >, aoc::exceptions::AocException> readLists(
        const std::filesystem::path &path) noexcept;

    template<aoc::templates::Numeric T>
    [[nodiscard]] static bool isSafe(std::span<const T> list);

    static inline std::filesystem::path INPUT_FILE{std::filesystem::path{"../data"} / "d2p1.txt"};
};

inline void Day02::partOne() {
    auto lines = readLists<int64_t>(INPUT_FILE);
    if (!lines) {
        std::println("Error reading lists: {}", lines.error().what());
        return;
    }

    size_t safeNum = 0;

    for (auto line: lines.value()) {
        if (isSafe<int64_t>(line)) safeNum++;
    }

    std::println("Number of safe lines: {}", safeNum);
}

inline void Day02::partTwoBruteForce() {
    auto lines = readLists<int64_t>(INPUT_FILE);
    if (!lines) {
        std::println("Error reading lists: {}", lines.error().what());
        return;
    }

    const size_t safeNum = countSafeParallel(lines.value(), isSafeWithChance<int64_t>);

    std::println("Number of safe lines (brute force): {}", safeNum);
}

inline void Day02::partTwoSmart() {
    auto lines = readLists<int64_t>(INPUT_FILE);
    if (!lines) {
        std::println("Error reading lists: {}", lines.error().what());
        return;
    }

    const size_t safeNum = countSafeParallel(lines.value(), canBeMadeSafe<int64_t>);

    std::println("Number of safe lines (smart): {}", safeNum);
}

template<aoc::templates::Numeric T, aoc::templates::VectorOperation<T> Op>
size_t Day02::countSafeParallel(const std::vector<std::vector<T> > &lines, Op op) {
    return std::transform_reduce(
        std::execution::par_unseq,
        lines.begin(), lines.end(),
        size_t{0},
        std::plus{},
        [op](const auto &line) {
            return op(std::span{line}) ? 1 : 0;
        }
    );
}

template<aoc::templates::Numeric T>
bool Day02::rotateAndCheckSafety(std::vector<T> &testVec, std::span<const T> list, size_t i) {
    std::ranges::rotate(testVec.begin() + i, testVec.begin() + i + 1, testVec.end());
    testVec.pop_back();

    if (isSafe<T>(testVec)) {
        return true;
    }

    testVec.push_back(list[i]);
    std::ranges::rotate(testVec.rbegin(), testVec.rbegin() + 1, testVec.rend() - i);
    return false;
}

template<aoc::templates::Numeric T>
bool Day02::isSafeWithChance(std::span<const T> list) {
    if (list.size() < 2) return true;
    if (isSafe<T>(list)) return true;
    std::vector<T> testVec = list | std::ranges::to<std::vector>();
    for (uint32_t i = 0; i < list.size(); i++) {
        if (rotateAndCheckSafety(testVec, list, i)) {
            return true;
        }
    }

    return false;
}

template<aoc::templates::Numeric T>
bool Day02::canBeMadeSafe(std::span<const T> list) {
    if (list.size() < 2) return true;
    if (isSafe<T>(list)) return true;

    boost::container::small_vector<size_t, 10> problematicPositions;

    // Check for direction changes using adjacent triplets
    for (const auto &[a, b, c]: std::views::adjacent<3>(list)) {
        T curr_diff = b - a;
        T next_diff = c - b;
        if (curr_diff * next_diff <= 0) {
            size_t i = &a - list.data(); // Get position
            problematicPositions.insert(problematicPositions.end(), {i, i + 1, i + 2});
        }
    }

    // Check for invalid differences between pairs
    for (const auto &[a, b]: std::views::adjacent<2>(list)) {
        T curr_diff = b - a;
        if (std::abs(curr_diff) > 3 || curr_diff == 0) {
            size_t i = &a - list.data();
            problematicPositions.insert(problematicPositions.end(), {i, i + 1});
        }
    }

    // Remove duplicates while maintaining order
    std::ranges::sort(problematicPositions);
    problematicPositions.erase(
        std::ranges::unique(problematicPositions).begin(),
        problematicPositions.end()
    );

    std::vector<T> testVec = std::ranges::to<std::vector>(list);
    for (size_t pos: problematicPositions) {
        if (rotateAndCheckSafety(testVec, list, pos)) {
            return true;
        }
    }

    return false;
}

inline std::expected<std::ifstream, aoc::exceptions::AocException> Day02::openFile(
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

template<aoc::templates::Numeric T>
std::expected<std::vector<std::vector<T> >, aoc::exceptions::AocException> Day02::readLists(
    const std::filesystem::path &path) noexcept {
    auto stream = openFile(path);
    if (!stream) {
        return std::unexpected(stream.error());
    }
    std::vector<std::vector<T> > allLines;

    try {
        std::string line;
        while (std::getline(stream.value(), line)) {
            std::vector<T> numbers;
            auto view = std::string_view{line} | std::views::split(' ')
                        | std::views::filter([](auto v) { return !std::ranges::empty(v); });

            for (const auto &numStr: view) {
                T value;
                auto *ptr = numStr.data();
                auto [p, ec] = std::from_chars(ptr, ptr + numStr.size(), value);
                if (ec != std::errc{}) {
                    return std::unexpected(aoc::exceptions::DataFormatError("Invalid number format"));
                }
                numbers.push_back(value);
            }

            if (!numbers.empty()) {
                allLines.push_back(std::move(numbers));
            }
        }
    } catch (const std::bad_expected_access<T> &) {
        return std::unexpected(aoc::exceptions::DataFormatError("Stream in invalid state"));
    } catch (const std::ios_base::failure &) {
        return std::unexpected(aoc::exceptions::DataFormatError("Invalid number format"));
    } catch (const std::exception &) {
        return std::unexpected(aoc::exceptions::DataFormatError("Error reading numbers"));
    }

    return allLines;
}

template<aoc::templates::Numeric T>
bool Day02::isSafe(std::span<const T> list) {
    if (list.size() < 2) return true;

    bool isDecreasing = list[0] > list[1];
    return std::ranges::all_of(
        std::views::adjacent<2>(list),
        [isDecreasing](const auto &pair) {
            const auto [first, second] = pair;
            T diff = second - first;
            return diff != 0 &&
                   std::abs(diff) < 4 &&
                   (isDecreasing ? diff < 0 : diff > 0);
        }
    );
}
