#pragma once

#include <vector>
#include <string>
#include <array>
#include <filesystem>
#include <execution>
#include <fstream>
#include <print>
#include <ranges>
#include <span>
#include <algorithm>
#include <expected>

#include "AocExceptions.h"

class Day04 {
public:
    Day04() = delete; // This class is not meant to be instantiated
    ~Day04() = delete; // No inheritance either

    static void partOne();

    static void partTwo();

#ifdef TESTING
    friend class Day04Test;
#endif

private:
    // Used for both parts
    static inline std::filesystem::path INPUT_FILE{std::filesystem::path{"../data"} / "d4p1.txt"};
    static inline std::string_view target = "XMAS";

    // Part 1
    static constexpr auto directions = std::to_array<std::uint8_t>({
        0x11, // (1,1)   diagonal
        0x10, // (1,0)   right
        0x1F, // (1,-1)  diagonal up
        0x01, // (0,1)   down
        0x0F, // (0,-1)  up
        0xF1, // (-1,1)  diagonal
        0xF0, // (-1,0)  left
        0xFF // (-1,-1) diagonal up
    });

    struct MatchResult {
        bool valid;
        std::size_t currIdx;

        static constexpr MatchResult failure() noexcept;

        static constexpr MatchResult success(std::size_t idx) noexcept;
    };

    struct SearchTask {
        std::size_t startIdx;

        constexpr explicit SearchTask(std::size_t idx) noexcept;
    };

    // Used for both parts
    // --------------------------------------------------------------------------------------------- //
    static std::pair<std::vector<char>, size_t> getDataArray(std::ifstream &file) noexcept;

    static inline std::expected<std::ifstream, aoc::exceptions::AocException> openFile(
        const std::filesystem::path &path) noexcept;

    [[nodiscard]] static constexpr std::size_t toIndex(size_t numCols, std::size_t i,
                                                       std::size_t j) noexcept;

    [[nodiscard]] static constexpr auto fromIndex(size_t numCols, std::size_t idx) noexcept;

    // Part 1
    // --------------------------------------------------------------------------------------------- //
    [[nodiscard]] static constexpr auto decodeDirection(std::uint8_t dir) noexcept;

    [[nodiscard]] static constexpr MatchResult checkPattern(
        const std::vector<char> &data,
        std::size_t startIdx,
        std::ptrdiff_t stride
    ) noexcept;

    [[nodiscard]] static std::vector<std::tuple<std::size_t, std::size_t, std::size_t> >
    processSearchTask(const SearchTask &task, const std::vector<char> &data, std::size_t rows,
                      std::size_t cols) noexcept;

    [[nodiscard]] static std::vector<std::tuple<std::size_t, std::size_t, std::size_t> > findAll(
        const std::vector<char> &data, const std::vector<size_t> &startPositions, size_t rows, size_t cols
    ) noexcept;

    // Part 2
    // --------------------------------------------------------------------------------------------- //
    [[nodiscard]] static size_t checkPatternBlock(const std::vector<char> &data, size_t rows, size_t cols,
                                                  std::span<const size_t> positions) noexcept;

    [[nodiscard]] static size_t countPatterns(const std::vector<char> &data, size_t rows, size_t cols,
                                              const std::vector<size_t> &centerAPositions) noexcept;
};

inline void Day04::partOne() {
    auto file = openFile(INPUT_FILE);
    if (!file) {
        std::println("Error opening file: {}", file.error().what());
        return;
    }
    auto [data, rows] = getDataArray(file.value());
    const auto cols = data.size() / rows;
    std::vector<std::size_t> startPositions;
    startPositions.reserve(data.size() / 9); // Rough estimate
    for (std::size_t i = 0; i < data.size(); ++i) {
        if (data[i] == 'X') {
            startPositions.push_back(i);
        }
    }

    const auto results = findAll(data, startPositions, rows, cols);

    std::println("Matches: {}", results.size());
}

inline void Day04::partTwo() {
    auto file = openFile(INPUT_FILE);
    if (!file) {
        std::println("Error opening file: {}", file.error().what());
        return;
    }
    auto [data, rows] = getDataArray(file.value());
    const auto cols = data.size() / rows;
    std::vector<size_t> centerAPositions;
    centerAPositions.reserve(data.size() / 9); // Rough estimate

    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == 'A') {
            centerAPositions.push_back(i);
        }
    }
    const auto matches = countPatterns(data, rows, cols, centerAPositions);

    std::println("Pattern Matches: {}", matches);
}

constexpr Day04::MatchResult Day04::MatchResult::failure() noexcept { return {false, 0}; }

constexpr Day04::MatchResult Day04::MatchResult::success(const std::size_t idx) noexcept { return {true, idx}; }

constexpr Day04::SearchTask::SearchTask(const std::size_t idx) noexcept: startIdx(idx) {
}

inline std::pair<std::vector<char>, size_t> Day04::getDataArray(std::ifstream &file) noexcept {
    file.seekg(0, std::ios::end);
    const auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> data(fileSize);
    file.read(data.data(), fileSize);

    size_t rowCounter = std::ranges::count(data, '\n') + 1;

    std::erase(data, '\n');

    return {std::move(data), rowCounter};
}

inline std::expected<std::ifstream, aoc::exceptions::AocException> Day04::openFile(
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

constexpr std::size_t Day04::toIndex(const size_t numCols, const std::size_t i, const std::size_t j) noexcept {
    return i * numCols + j;
}

constexpr auto Day04::fromIndex(const size_t numCols, const std::size_t idx) noexcept {
    return std::pair{idx / numCols, idx % numCols};
}

constexpr auto Day04::decodeDirection(const std::uint8_t dir) noexcept {
    static constexpr std::uint8_t LOW_NIBBLE_MASK = 0x0F;
    static constexpr std::uint8_t SIGN_BIT_X = 0x08;
    static constexpr std::uint8_t SIGN_BIT_Y = 0x80;
    static constexpr std::uint8_t NEGATIVE_EXTENSION = 0xF0;

    // Extract and sign-extend x component
    const bool negativeX = (dir & SIGN_BIT_X) != 0;
    const auto dx = static_cast<std::int8_t>(
        (dir & LOW_NIBBLE_MASK) | (negativeX ? NEGATIVE_EXTENSION : 0)
    );

    // Extract and sign-extend y component
    const bool negativeY = (dir & SIGN_BIT_Y) != 0;
    const auto dy = static_cast<std::int8_t>(
        (dir >> 4) | (negativeY ? NEGATIVE_EXTENSION : 0)
    );

    return std::pair{dx, dy};
}

constexpr Day04::MatchResult Day04::checkPattern(const std::vector<char> &data, const std::size_t startIdx,
                                                 const std::ptrdiff_t stride) noexcept {
    auto currIdx = startIdx;
    for (std::size_t k = 1; k < target.length(); ++k) {
        currIdx = static_cast<std::size_t>(static_cast<std::ptrdiff_t>(currIdx) + stride);
        if (data[currIdx] != target[k]) {
            return MatchResult::failure();
        }
    }
    return MatchResult::success(currIdx);
}

inline std::vector<std::tuple<std::size_t, std::size_t, std::size_t> > Day04::processSearchTask(const SearchTask &task,
    const std::vector<char> &data, const std::size_t rows, const std::size_t cols) noexcept {
    std::vector<std::tuple<std::size_t, std::size_t, std::size_t> > localResults;

    auto [startI, startJ] = fromIndex(cols, task.startIdx);

    if (data[task.startIdx] != target[0]) {
        return localResults;
    }

    for (std::size_t dir = 0; dir < directions.size(); ++dir) {
        auto [dx, dy] = decodeDirection(directions[dir]);

        const auto targetLen = static_cast<std::ptrdiff_t>(target.length() - 1);
        const auto endI = static_cast<std::ptrdiff_t>(startI) + dx * targetLen;
        if (const auto endJ = static_cast<std::ptrdiff_t>(startJ) + dy * targetLen;
            endI < 0 || static_cast<std::size_t>(endI) >= rows ||
            endJ < 0 || static_cast<std::size_t>(endJ) >= cols) {
            continue;
        }

        const std::ptrdiff_t stride = static_cast<std::ptrdiff_t>(dx) *
                                      static_cast<std::ptrdiff_t>(cols) +
                                      static_cast<std::ptrdiff_t>(dy);

        if (checkPattern(data, task.startIdx, stride).valid) {
            localResults.emplace_back(startI, startJ, dir);
        }
    }
    return localResults;
}

inline std::vector<std::tuple<std::size_t, std::size_t, std::size_t> > Day04::findAll(const std::vector<char> &data,
    const std::vector<size_t> &startPositions, const size_t rows, const size_t cols) noexcept {
    std::vector<SearchTask> tasks;
    tasks.reserve(startPositions.size());

    for (const auto idx: startPositions) {
        tasks.emplace_back(idx);
    }

    std::vector<std::vector<std::tuple<std::size_t, std::size_t, std::size_t> > >
            allResults(tasks.size());

    std::transform(
        std::execution::par_unseq,
        tasks.begin(), tasks.end(),
        allResults.begin(),
        [&data, rows, cols](const SearchTask &task) {
            return processSearchTask(task, data, rows, cols);
        }
    );

    // Calculate total size needed for final results
    const auto totalSize = std::accumulate(
        allResults.begin(),
        allResults.end(),
        std::size_t{0},
        [](std::size_t sum, const auto &resultSet) {
            return sum + resultSet.size();
        }
    );

    std::vector<std::tuple<std::size_t, std::size_t, std::size_t> > finalResults;
    finalResults.reserve(totalSize);

    for (auto &resultSet: allResults) {
        finalResults.insert(
            finalResults.end(),
            std::make_move_iterator(resultSet.begin()),
            std::make_move_iterator(resultSet.end())
        );
    }

    return finalResults;
}

inline size_t Day04::checkPatternBlock(const std::vector<char> &data, const size_t rows, const size_t cols,
                                       std::span<const size_t> positions) noexcept {
    size_t matches = 0;

    for (const auto pos: positions) {
        const auto [row, col] = fromIndex(cols, pos);

        // Quick bounds check
        if (row == 0 || row + 1 >= rows || col == 0 || col + 1 >= cols) continue;

        // Get the characters in the diagonals
        const char topLeft = data[toIndex(cols, row - 1, col - 1)];
        const char topRight = data[toIndex(cols, row - 1, col + 1)];
        const char bottomLeft = data[toIndex(cols, row + 1, col - 1)];
        const char bottomRight = data[toIndex(cols, row + 1, col + 1)];

        // Check if diagonals form MS patterns
        const bool topLeftBottomRight = (topLeft == 'M' && bottomRight == 'S') ||
                                        (topLeft == 'S' && bottomRight == 'M');

        const bool topRightBottomLeft = (topRight == 'M' && bottomLeft == 'S') ||
                                        (topRight == 'S' && bottomLeft == 'M');

        matches += (topLeftBottomRight && topRightBottomLeft) ? 1 : 0;
    }
    return matches;
}

inline size_t Day04::countPatterns(const std::vector<char> &data, const size_t rows, const size_t cols,
                                   const std::vector<size_t> &centerAPositions) noexcept {
    constexpr size_t BLOCK_SIZE = 64;
    size_t totalMatches = 0;

    const std::span positions(centerAPositions);
    for (size_t i = 0; i < positions.size(); i += BLOCK_SIZE) {
        const size_t blockCount = std::min(BLOCK_SIZE, positions.size() - i);
        totalMatches += checkPatternBlock(data, rows, cols, positions.subspan(i, blockCount));
    }

    return totalMatches;
}
