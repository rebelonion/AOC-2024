#include <fstream>

#include <gtest/gtest.h>

#include "Day02.h"

class Day02Test : public ::testing::Test {
protected:
    static std::filesystem::path createTempFile(const std::string& content) {
        auto path = std::filesystem::temp_directory_path() / "test_input.txt";
        std::ofstream file(path);
        file << content;
        return path;
    }

    static void TestReadListsValid() {
        const auto path = createTempFile("1 2 3\n4 5 6\n7 8 9");
        const auto result = Day02::readLists<int64_t>(path);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result->size(), 3);
        EXPECT_EQ((*result)[0], (std::vector<int64_t>{1, 2, 3}));
        EXPECT_EQ((*result)[1], (std::vector<int64_t>{4, 5, 6}));
        EXPECT_EQ((*result)[2], (std::vector<int64_t>{7, 8, 9}));
    }

    static void TestReadListsInvalid() {
        const auto path = createTempFile("1 2 3\nabc\n7 8 9");
        const auto result = Day02::readLists<int64_t>(path);
        ASSERT_FALSE(result.has_value());
    }

    static void TestIsSafe() {
        // Test strictly increasing sequence
        std::vector<int64_t> increasing{1, 2, 3};
        EXPECT_TRUE(Day02::isSafe<int64_t>(std::span{increasing}));

        // Test strictly decreasing sequence
        std::vector<int64_t> decreasing{3, 2, 1};
        EXPECT_TRUE(Day02::isSafe<int64_t>(std::span{decreasing}));

        // Test invalid sequence (not monotonic)
        std::vector<int64_t> nonMonotonic{1, 3, 2};
        EXPECT_FALSE(Day02::isSafe<int64_t>(std::span{nonMonotonic}));

        // Test sequence with too large difference
        std::vector<int64_t> largeDiff{1, 5, 9};
        EXPECT_FALSE(Day02::isSafe<int64_t>(std::span{largeDiff}));

        // Test sequence with zero difference
        std::vector<int64_t> zeroDiff{1, 1, 2};
        EXPECT_FALSE(Day02::isSafe<int64_t>(std::span{zeroDiff}));

        // Test single element (should be safe)
        std::vector<int64_t> single{1};
        EXPECT_TRUE(Day02::isSafe<int64_t>(std::span{single}));

        // Test empty sequence (should be safe)
        std::vector<int64_t> empty;
        EXPECT_TRUE(Day02::isSafe<int64_t>(std::span{empty}));
    }

    static void TestIsSafeWithChance() {
        // Test sequence that's already safe
        std::vector<int64_t> safe{1, 2, 3};
        EXPECT_TRUE(Day02::isSafeWithChance<int64_t>(std::span{safe}));

        // Test sequence that can be made safe by removing middle element
        std::vector<int64_t> canBeMadeSafe{1, 5, 2};
        EXPECT_TRUE(Day02::isSafeWithChance<int64_t>(std::span{canBeMadeSafe}));

        // Test sequence that cannot be made safe
        std::vector<int64_t> cannotBeMadeSafe{1, 5, 2, 6, 3};
        EXPECT_FALSE(Day02::isSafeWithChance<int64_t>(std::span{cannotBeMadeSafe}));
    }

    static void TestCanBeMadeSafe() {
        // Test sequence with direction change
        std::vector<int64_t> directionChange{1, 3, 2};
        EXPECT_TRUE(Day02::canBeMadeSafe<int64_t>(std::span{directionChange}));

        // Test sequence with invalid difference
        std::vector<int64_t> invalidDiff{1, 5, 6};
        EXPECT_TRUE(Day02::canBeMadeSafe<int64_t>(std::span{invalidDiff}));

        // Test sequence with multiple issues that can be fixed
        std::vector<int64_t> multipleIssues{1, 4, 2, 5};
        EXPECT_TRUE(Day02::canBeMadeSafe<int64_t>(std::span{multipleIssues}));

        // Test sequence that cannot be made safe
        std::vector<int64_t> impossible{1, 5, 2, 6, 3, 7, 4};
        EXPECT_FALSE(Day02::canBeMadeSafe<int64_t>(std::span{impossible}));
    }

    static void TestCountSafeParallel() {
        const std::vector<std::vector<int64_t>> testData{
            {1, 2, 3},    // safe
            {3, 2, 1},    // safe
            {1, 5, 2},    // unsafe but can be made safe
            {1, 1, 1},    // unsafe and cannot be made safe
        };

        const size_t safeCount = Day02::countSafeParallel(
            testData,
            Day02::isSafe<int64_t>
        );
        EXPECT_EQ(safeCount, 2);

        const size_t canBeMadeSafeCount = Day02::countSafeParallel(
            testData,
            Day02::canBeMadeSafe<int64_t>
        );
        EXPECT_EQ(canBeMadeSafeCount, 3);
    }
};

TEST_F(Day02Test, ReadListsValid) {
    TestReadListsValid();
}

TEST_F(Day02Test, ReadListsInvalid) {
    TestReadListsInvalid();
}

TEST_F(Day02Test, IsSafe) {
    TestIsSafe();
}

TEST_F(Day02Test, IsSafeWithChance) {
    TestIsSafeWithChance();
}

TEST_F(Day02Test, CanBeMadeSafe) {
    TestCanBeMadeSafe();
}

TEST_F(Day02Test, CountSafeParallel) {
    TestCountSafeParallel();
}