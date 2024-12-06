
#include <gtest/gtest.h>

#include "Day05.h"

class Day05Test : public ::testing::Test {
protected:
    static std::filesystem::path createTempFile(const std::string& content) {
        auto path = std::filesystem::temp_directory_path() / "test_input.txt";
        std::ofstream file(path);
        file << content;
        return path;
    }

    static void TestReadListsValid() {
        // Test reading comma-separated lists
        const auto path = createTempFile("1,2,3\n4,5,6\n7,8,9");
        const auto result = Day05::readLists<int>(path, ',');
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result->size(), 3);
        EXPECT_EQ((*result)[0], (std::vector{1, 2, 3}));
        EXPECT_EQ((*result)[1], (std::vector{4, 5, 6}));
        EXPECT_EQ((*result)[2], (std::vector{7, 8, 9}));

        // Test reading pipe-separated lists
        const auto pathPipe = createTempFile("1|2\n3|4\n5|6");
        const auto resultPipe = Day05::readLists<int>(pathPipe, '|');
        ASSERT_TRUE(resultPipe.has_value());
        EXPECT_EQ(resultPipe->size(), 3);
        EXPECT_EQ((*resultPipe)[0], (std::vector<int>{1, 2}));
    }

    static void TestReadListsInvalid() {
        // Test invalid number format
        const auto pathInvalid = createTempFile("1,2,abc\n4,5,6");
        const auto resultInvalid = Day05::readLists<int>(pathInvalid, ',');
        ASSERT_FALSE(resultInvalid.has_value());

        // Test empty file
        const auto pathEmpty = createTempFile("");
        const auto resultEmpty = Day05::readLists<int>(pathEmpty, ',');
        ASSERT_TRUE(resultEmpty.has_value());
        EXPECT_TRUE(resultEmpty->empty());
    }

    static void TestBuildRuleMap() {
        // Test valid rule building
        const auto path = createTempFile("1|2\n3|4\n1|5");
        auto ruleMap = Day05::buildRuleMap(path.string());
        ASSERT_TRUE(ruleMap.has_value());
        EXPECT_EQ(ruleMap->size(), 2);
        EXPECT_EQ((*ruleMap)[1].size(), 2);
        EXPECT_TRUE((*ruleMap)[1].contains(2));
        EXPECT_TRUE((*ruleMap)[1].contains(5));
        EXPECT_EQ((*ruleMap)[3].size(), 1);
        EXPECT_TRUE((*ruleMap)[3].contains(4));

        // Test invalid rule format
        const auto pathInvalid = createTempFile("1|2|3\n4|5");
        auto ruleMapInvalid = Day05::buildRuleMap(pathInvalid.string());
        ASSERT_FALSE(ruleMapInvalid.has_value());
    }

    static void TestBreaksRule() {
        const std::unordered_set rules{2, 4, 6};

        // Test rule breaking
        std::vector numbers1{1, 2, 3};
        auto [breaks1, index1] = Day05::breaksRule(rules, std::span{numbers1});
        EXPECT_TRUE(breaks1);
        EXPECT_EQ(index1, 1);

        // Test no rule breaking
        std::vector numbers2{1, 3, 5};
        auto [breaks2, index2] = Day05::breaksRule(rules, std::span{numbers2});
        EXPECT_FALSE(breaks2);
        EXPECT_EQ(index2, 0);

        // Test empty numbers
        std::vector<int> empty;
        auto [breaksEmpty, indexEmpty] = Day05::breaksRule(rules, std::span{empty});
        EXPECT_FALSE(breaksEmpty);
        EXPECT_EQ(indexEmpty, 0);
    }

    static void TestFixList() {
        // Test successful fix
        std::vector list1{1, 2, 3, 4, 5};
        EXPECT_TRUE(Day05::fixList(list1, 1, 4));
        EXPECT_EQ(list1, (std::vector{1, 4, 2, 3, 5}));

        // Test fix when value not found
        std::vector list2{1, 2, 3};
        EXPECT_FALSE(Day05::fixList(list2, 1, 4));
        EXPECT_EQ(list2, (std::vector{1, 2, 3}));
    }

    static void TestProcessUpdate() {
        // Setup rule map
        std::unordered_map<int, std::unordered_set<int>> ruleMap;
        ruleMap[3] = std::unordered_set{1, 2};
        ruleMap[5] = std::unordered_set{4};

        // Test valid update without broken rules
        std::vector update1{3, 1, 5};
        auto result1 = Day05::processUpdate(update1, ruleMap, false);
        ASSERT_TRUE(result1.has_value());
        EXPECT_EQ(result1.value(), 1);

        // Test update with broken rules (Part 1 behavior)
        std::vector update2{1, 3, 5};
        auto result2 = Day05::processUpdate(update2, ruleMap, false);
        ASSERT_TRUE(result2.has_value());
        EXPECT_EQ(result2.value(), 0);

        // Test update with broken rules (Part 2 behavior - fixable)
        std::vector update3{1, 3, 5};
        auto result3 = Day05::processUpdate(update3, ruleMap, true);
        ASSERT_TRUE(result3.has_value());
        EXPECT_EQ(result3.value(), 1);

        // Test invalid update (even length)
        std::vector update4{3, 1, 2, 4};
        auto result4 = Day05::processUpdate(update4, ruleMap, false);
        ASSERT_FALSE(result4.has_value());
    }

    static void TestGetMiddleValue() {
        // Test odd-length list
        const std::vector list1{1, 2, 3};
        const auto result1 = Day05::getMiddleValue(list1);
        ASSERT_TRUE(result1.has_value());
        EXPECT_EQ(result1.value(), 2);

        // Test even-length list
        const std::vector list2{1, 2, 3, 4};
        const auto result2 = Day05::getMiddleValue(list2);
        ASSERT_FALSE(result2.has_value());

        // Test single-element list
        const std::vector list3{1};
        const auto result3 = Day05::getMiddleValue(list3);
        ASSERT_TRUE(result3.has_value());
        EXPECT_EQ(result3.value(), 1);
    }
};

TEST_F(Day05Test, ReadListsValid) {
    TestReadListsValid();
}

TEST_F(Day05Test, ReadListsInvalid) {
    TestReadListsInvalid();
}

TEST_F(Day05Test, BuildRuleMap) {
    TestBuildRuleMap();
}

TEST_F(Day05Test, BreaksRule) {
    TestBreaksRule();
}

TEST_F(Day05Test, FixList) {
    TestFixList();
}

TEST_F(Day05Test, ProcessUpdate) {
    TestProcessUpdate();
}

TEST_F(Day05Test, GetMiddleValue) {
    TestGetMiddleValue();
}