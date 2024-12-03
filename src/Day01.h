#pragma once

#include <expected>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <print>
#include <unordered_map>
#include <vector>

#include <bits/ranges_algo.h>

#include "AocExceptions.h"
#include "AocTemplates.h"

class Day01 {
public:
    Day01() = delete; // This class is not meant to be instantiated
    ~Day01() = delete; // No inheritance either

    template<aoc::templates::Numeric T>
    struct NumberLists {  // Cleaner passing of two lists
        std::vector<T> left;
        std::vector<T> right;

        [[nodiscard]] constexpr NumberLists(std::vector<T> l, std::vector<T> r) noexcept;

        [[nodiscard]] size_t size() const noexcept;
    };

    static void partOne();

    static void partTwo();

#ifdef TESTING
    friend class Day01Test;
#endif

private:
    template<aoc::templates::Numeric T, aoc::templates::ListBinaryOperation<T> BinaryOp>
    [[nodiscard]] static T calculateWithLists(std::span<const T> left, std::span<const T> right, BinaryOp op) noexcept;

    template<aoc::templates::Numeric T>
    static std::expected<NumberLists<T>, aoc::exceptions::AocException> readLists(
        const std::filesystem::path &path) noexcept;

    static std::expected<std::ifstream, aoc::exceptions::AocException> openFile(
        const std::filesystem::path &path) noexcept;

    static inline std::filesystem::path INPUT_FILE{std::filesystem::path{"../data"} / "d1p1.txt"};
};

template<aoc::templates::Numeric T>
constexpr Day01::NumberLists<T>::NumberLists(std::vector<T> l, std::vector<T> r) noexcept: left(std::move(l)),
    right(std::move(r)) {
}

template<aoc::templates::Numeric T>
size_t Day01::NumberLists<T>::size() const noexcept { return left.size(); }

inline void Day01::partOne() {
    auto lists = readLists<int64_t>(INPUT_FILE);
    if (!lists) {
        std::println("Error reading lists: {}", lists.error().what());
        return;
    }
    //Simple ordered sorting
    std::ranges::sort(lists->left);
    std::ranges::sort(lists->right);

    auto total = calculateWithLists<int64_t>(lists->left, lists->right,
                                             [](const int64_t a, const int64_t b) { return std::abs(a - b); });

    std::println("Total is {}", total);
}

inline void Day01::partTwo() {
    const auto lists = readLists<int64_t>(INPUT_FILE);
    if (!lists) {
        std::println("Error reading lists: {}", lists.error().what());
        return;
    }

    // Create frequency map
    std::unordered_map<int64_t, int64_t> frequency;
    frequency.reserve(lists->size());
    for (const int64_t num: lists->right) {
        frequency[num]++;
    }
    // O(1) lookup
    auto similarityScore =
            calculateWithLists<int64_t>(lists->left, lists->right, [&frequency](const int64_t left, const int64_t) {
                return left * frequency[left];
            });

    std::println("Similarity score is {}", similarityScore);
}

template<aoc::templates::Numeric T, aoc::templates::ListBinaryOperation<T> BinaryOp>
T Day01::calculateWithLists(std::span<const T> left, std::span<const T> right, BinaryOp op) noexcept {
    return std::transform_reduce(
        left.begin(), left.end(), // First range
        right.begin(), // Second range
        T{0}, // Initial value
        std::plus(), // Reduction operation
        op // Transform operation (lambda)
    );
}

template<aoc::templates::Numeric T>
std::expected<Day01::NumberLists<T>, aoc::exceptions::AocException> Day01::readLists(
    const std::filesystem::path &path) noexcept {
    auto stream = openFile(path);
    if (!stream) {
        return std::unexpected(stream.error());
    }
    std::vector<T> listOne;
    std::vector<T> listTwo;
    T num1;
    T num2;

    //Directly read the two numbers into the vectors
    try {
        while (true) {
            if (!(stream.value() >> num1)) {
                // End of file is OK, other failures are errors
                if (!stream.value().eof()) {
                    return std::unexpected(aoc::exceptions::DataFormatError("Stream in invalid state"));
                }
                break;
            }
            // If we got num1 but can't get num2, that's an error
            if (!(stream.value() >> num2)) {
                return std::unexpected(aoc::exceptions::DataFormatError("Stream in invalid state"));
            }
            listOne.push_back(num1);
            listTwo.push_back(num2);
        }
    } catch (const std::bad_expected_access<T>&) {
        return std::unexpected(aoc::exceptions::DataFormatError("Stream in invalid state"));
    } catch (const std::ios_base::failure&) {
        return std::unexpected(aoc::exceptions::DataFormatError("Invalid number format"));
    } catch (const std::exception&) {
        return std::unexpected(aoc::exceptions::DataFormatError("Error reading numbers"));
    }

    //If the vectors are not equal, something went wrong reading the data
    if (listOne.size() != listTwo.size()) {
        return std::unexpected(aoc::exceptions::DataFormatError(
            std::format("vectors are not equal: {} != {}", listOne.size(), listTwo.size())));
    }

    return NumberLists<T>(std::move(listOne), std::move(listTwo));
}

inline std::expected<std::ifstream, aoc::exceptions::AocException> Day01::openFile(
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
