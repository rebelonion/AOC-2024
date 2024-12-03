#include <gtest/gtest.h>

#include "Day01.h"


class Day01Test : public ::testing::Test {
protected:
    static std::filesystem::path createTempFile(const std::string& content) {
        auto path = std::filesystem::temp_directory_path() / "test_input.txt";
        std::ofstream file(path);
        file << content;
        return path;
    }

    static void TestReadListsValid() {
        const auto path = createTempFile("1 2\n3 4\n5 6");
        const auto result = Day01::readLists<int64_t>(path);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result->size(), 3);
        EXPECT_EQ(result->left, (std::vector<int64_t>{1, 3, 5}));
        EXPECT_EQ(result->right, (std::vector<int64_t>{2, 4, 6}));
    }

    static void TestReadListsInvalid() {
        const auto path = createTempFile("1 2\n3\n5 6");
        const auto result = Day01::readLists<int64_t>(path);
        ASSERT_FALSE(result.has_value());
    }

    static void TestCalculateWithLists() {
        std::vector<int64_t> left{1, 2, 3};
        std::vector<int64_t> right{4, 5, 6};

        const auto result = Day01::calculateWithLists<int64_t>(
                std::span{left},
                std::span{right},
                [](const int64_t a, const int64_t b) { return std::abs(a - b); }
        );

        EXPECT_EQ(result, 9); // |1-4| + |2-5| + |3-6| = 3 + 3 + 3 = 9
    }

    static void TestNumberListsConstruction() {
        std::vector<int64_t> left{1, 2, 3};
        std::vector<int64_t> right{4, 5, 6};
        const Day01::NumberLists lists(std::move(left), std::move(right));
        EXPECT_EQ(lists.size(), 3);
    }

    static void TestSimilarityScore() {
        const auto path = createTempFile("2 2\n2 3\n3 2");
        auto lists = Day01::readLists<int64_t>(path);
        ASSERT_TRUE(lists.has_value());

        std::unordered_map<int64_t, int64_t> frequency;
        for (const auto num : lists->right) {
            frequency[num]++;
        }

        const auto score = Day01::calculateWithLists<int64_t>(
                std::span{lists->left},
                std::span{lists->right},
                [&frequency](const int64_t left, int64_t) { return left * frequency[left]; }
        );

        EXPECT_EQ(score, 11); // 2*2 + 2*2 + 3*1 = 11
    }
};

TEST_F(Day01Test, ReadListsValid) {
    TestReadListsValid();
}

TEST_F(Day01Test, ReadListsInvalid) {
    TestReadListsInvalid();
}

TEST_F(Day01Test, CalculateWithLists) {
    TestCalculateWithLists();
}

TEST_F(Day01Test, NumberListsConstruction) {
    TestNumberListsConstruction();
}

TEST_F(Day01Test, SimilarityScore) {
    TestSimilarityScore();
}