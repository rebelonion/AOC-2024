#pragma once

#include <expected>
#include <filesystem>
#include <fstream>
#include <print>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "AocExceptions.h"
#include "AocTemplates.h"

class Day05 {
public:
    Day05() = delete;

    ~Day05() = delete;

    static void partOne();

    static void partTwo();

#ifdef TESTING
    friend class Day05Test;
#endif

private:
    // Core processing logic shared between both parts
    static std::expected<size_t, aoc::exceptions::AocException> processPuzzle(bool fixBrokenRules);

    // Process a single update list and return its middle value if valid
    static std::expected<size_t, aoc::exceptions::AocException> processUpdate(
        std::vector<int> &update,
        const std::unordered_map<int, std::unordered_set<int> > &ruleMap,
        bool fixBrokenRules
    );

    // Helper to safely get the middle value of a list
    static std::expected<size_t, aoc::exceptions::AocException> getMiddleValue(const std::vector<int> &list);

    // Rule checking helper
    [[nodiscard]]
    static auto breaksRule(const std::unordered_set<int> &rules, std::span<const int> numbers) -> std::pair<bool, int>;

    // List fixing helper
    [[nodiscard]]
    static constexpr auto fixList(std::vector<int> &update, int targetIndex, int value) -> bool;

    // Rule map building helper
    static std::expected<std::unordered_map<int, std::unordered_set<int> >, aoc::exceptions::AocException>
    buildRuleMap(const std::string &filename);

    template<aoc::templates::Numeric T>
    static std::expected<std::vector<std::vector<T> >, aoc::exceptions::AocException> readLists(
        const std::filesystem::path &path, char splitter) noexcept;

    static inline std::expected<std::ifstream, aoc::exceptions::AocException> openFile(
        const std::filesystem::path &path) noexcept;

    static inline std::filesystem::path INPUT_FILE{std::filesystem::path{"../data"} / "d5p1.txt"};
    static inline std::filesystem::path INPUT_FILE_2{std::filesystem::path{"../data"} / "d5p2.txt"};
};

inline void Day05::partOne() {
    if (auto result = processPuzzle(false)) {
        std::println("Middle values sum: {}", result.value());
    }
}

inline void Day05::partTwo() {
    if (auto result = processPuzzle(true)) {
        std::println("Middle values sum: {}", result.value());
    }
}

inline std::expected<size_t, aoc::exceptions::AocException> Day05::processPuzzle(const bool fixBrokenRules) {
    auto ruleMap = buildRuleMap(INPUT_FILE);
    if (!ruleMap) return std::unexpected(ruleMap.error());

    auto updateLists = readLists<int>(INPUT_FILE_2, ',');
    if (!updateLists) return std::unexpected(updateLists.error());

    size_t middleValuesSum = 0;
    for (auto &update: updateLists.value()) {
        if (auto middleValue = processUpdate(update, ruleMap.value(), fixBrokenRules)) {
            middleValuesSum += middleValue.value();
        }
    }

    return middleValuesSum;
}

inline std::expected<size_t, aoc::exceptions::AocException> Day05::processUpdate(std::vector<int> &update,
    const std::unordered_map<int, std::unordered_set<int> > &ruleMap, const bool fixBrokenRules) {
    bool rulesBroken = false;

    const auto updateSize = update.size();
    for (size_t j = 0; j < updateSize; ++j) {
        const auto updateItem = update[j];
        if (!ruleMap.contains(updateItem)) continue;

        const auto &itemRules = ruleMap.at(updateItem);
        auto numbersBeforeItem = std::span(update.begin(), update.begin() + static_cast<int>(j));

        auto [hasBreak, breakIndex] = breaksRule(itemRules, numbersBeforeItem);
        if (!hasBreak) continue;
        if (!fixBrokenRules) return 0; // Part 1: skip broken rules

        // Part 2: attempt to fix the broken rules
        rulesBroken = true;
        while (hasBreak) {
            if (!fixList(update, breakIndex, updateItem)) {
                return std::unexpected(aoc::exceptions::DataFormatError("No fix found for broken rule"));
            }
            numbersBeforeItem = std::span(update.begin(), update.begin() + breakIndex);
            std::tie(hasBreak, breakIndex) = breaksRule(itemRules, numbersBeforeItem);
        }
    }

    // Only process lists that were originally valid (Part 1) or were fixed (Part 2)
    if ((!rulesBroken && !fixBrokenRules) || rulesBroken) {
        return getMiddleValue(update);
    }

    return 0;
}

inline std::expected<size_t, aoc::exceptions::AocException> Day05::getMiddleValue(const std::vector<int> &list) {
    if (list.size() % 2 != 1) {
        return std::unexpected(aoc::exceptions::DataFormatError("List has an even number of items"));
    }
    return list[list.size() / 2];
}

inline auto Day05::breaksRule(const std::unordered_set<int> &rules,
                              std::span<const int> numbers) -> std::pair<bool, int> {
    if (const auto it = std::ranges::find_if(numbers,
                                             [&rules](const auto &num) { return rules.contains(num); });
        it != numbers.end()) {
        return {true, static_cast<int>(std::distance(numbers.begin(), it))};
    }
    return {false, 0};
}

constexpr auto Day05::fixList(std::vector<int> &update, const int targetIndex, const int value) -> bool {
    if (const auto it = std::ranges::find(update, value); it != update.end()) {
        const auto distance = std::distance(update.begin(), it);
        std::rotate(
            update.begin() + targetIndex,
            update.begin() + distance,
            update.begin() + distance + 1
        );
        return true;
    }
    return false;
}

inline std::expected<std::unordered_map<int, std::unordered_set<int> >, aoc::exceptions::AocException> Day05::
buildRuleMap(const std::string &filename) {
    auto rules = readLists<int>(filename, '|');
    if (!rules) return std::unexpected(rules.error());

    std::unordered_map<int, std::unordered_set<int> > ruleMap;
    for (const auto &rule: rules.value()) {
        if (rule.size() != 2) {
            return std::unexpected(aoc::exceptions::DataFormatError("Invalid rule format"));
        }
        ruleMap[rule[0]].insert(rule[1]);
    }
    return ruleMap;
}

template<aoc::templates::Numeric T>
std::expected<std::vector<std::vector<T> >, aoc::exceptions::AocException> Day05::readLists(
    const std::filesystem::path &path, char splitter) noexcept {
    auto stream = openFile(path);
    if (!stream) {
        return std::unexpected(stream.error());
    }
    std::vector<std::vector<T> > allLines;

    try {
        std::string line;
        while (std::getline(stream.value(), line)) {
            std::vector<T> numbers;
            auto view = std::string_view{line} | std::views::split(splitter)
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

inline std::expected<std::ifstream, aoc::exceptions::AocException> Day05::openFile(
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
