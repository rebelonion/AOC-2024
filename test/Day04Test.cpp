#include <gtest/gtest.h>

#include "Day04.h"

class Day04Test : public ::testing::Test {
protected:
    static std::filesystem::path createTempFile(const std::string& content) {
        auto path = std::filesystem::temp_directory_path() / "test_input.txt";
        std::ofstream file(path);
        file << content;
        return path;
    }

    static void TestSimpleHorizontalPattern() {
        const std::string input = "XMAS\n....";
        const auto path = createTempFile(input);
        auto file = Day04::openFile(path);
        ASSERT_TRUE(file.has_value());

        auto [data, rows] = Day04::getDataArray(file.value());
        const auto cols = data.size() / rows;

        const std::vector<size_t> startPositions{0}; // X is at position 0
        const auto results = Day04::findAll(data, startPositions, rows, cols);

        EXPECT_EQ(results.size(), 1);
        const auto& [row, col, dir] = results[0];
        EXPECT_EQ(row, 0);
        EXPECT_EQ(col, 0);
        EXPECT_EQ(dir, 1); // Right direction
    }

    static void TestVerticalPattern() {
        const std::string input = "X...\nM...\nA...\nS...";
        const auto path = createTempFile(input);
        auto file = Day04::openFile(path);
        ASSERT_TRUE(file.has_value());

        auto [data, rows] = Day04::getDataArray(file.value());
        const auto cols = data.size() / rows;

        std::vector<size_t> startPositions{0};
        const auto results = Day04::findAll(data, startPositions, rows, cols);

        EXPECT_EQ(results.size(), 1);
        const auto& [row, col, dir] = results[0];
        EXPECT_EQ(row, 0);
        EXPECT_EQ(col, 0);
        EXPECT_EQ(dir, 3); // Down direction
    }

    static void TestDiagonalPattern() {
        const std::string input = "X....\n.M...\n..A..\n...S.";
        const auto path = createTempFile(input);
        auto file = Day04::openFile(path);
        ASSERT_TRUE(file.has_value());

        auto [data, rows] = Day04::getDataArray(file.value());
        const auto cols = data.size() / rows;

        std::vector<size_t> startPositions{0};
        const auto results = Day04::findAll(data, startPositions, rows, cols);

        EXPECT_EQ(results.size(), 1);
        const auto& [row, col, dir] = results[0];
        EXPECT_EQ(row, 0);
        EXPECT_EQ(col, 0);
        EXPECT_EQ(dir, 0); // Diagonal direction
    }

    static void TestMultiplePatterns() {
        const std::string input = "XMAS.XMAS\n....X....\n....M....\n....A....\n....S....";
        const auto path = createTempFile(input);
        auto file = Day04::openFile(path);
        ASSERT_TRUE(file.has_value());

        auto [data, rows] = Day04::getDataArray(file.value());
        const auto cols = data.size() / rows;

        std::vector<size_t> startPositions;
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == 'X') {
                startPositions.push_back(i);
            }
        }

        const auto results = Day04::findAll(data, startPositions, rows, cols);
        EXPECT_EQ(results.size(), 3); // Horizontal and vertical patterns
    }

    static void TestNoPattern() {
        const std::string input = "XAMX\n....";
        const auto path = createTempFile(input);
        auto file = Day04::openFile(path);
        ASSERT_TRUE(file.has_value());

        auto [data, rows] = Day04::getDataArray(file.value());
        const auto cols = data.size() / rows;

        const std::vector<size_t> startPositions{0};
        const auto results = Day04::findAll(data, startPositions, rows, cols);

        EXPECT_EQ(results.size(), 0);
    }

    static void TestPartTwoSimplePattern() {
        const std::string input = ".S.M.\n.AAS.\n.S.M.";
        const auto path = createTempFile(input);
        auto file = Day04::openFile(path);
        ASSERT_TRUE(file.has_value());

        auto [data, rows] = Day04::getDataArray(file.value());
        const auto cols = data.size() / rows;

        std::vector<size_t> centerAPositions;
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == 'A') {
                centerAPositions.push_back(i);
            }
        }

        const auto matches = Day04::countPatterns(data, rows, cols, centerAPositions);
        EXPECT_EQ(matches, 1);
    }

    static void TestPartTwoMultiplePatterns() {
        const std::string input = ".M.M..S.S.\n.AAS..AAS.\n.S.S..M.M.";
        const auto path = createTempFile(input);
        auto file = Day04::openFile(path);
        ASSERT_TRUE(file.has_value());

        auto [data, rows] = Day04::getDataArray(file.value());
        const auto cols = data.size() / rows;

        std::vector<size_t> centerAPositions;
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == 'A') {
                centerAPositions.push_back(i);
            }
        }

        const auto matches = Day04::countPatterns(data, rows, cols, centerAPositions);
        EXPECT_EQ(matches, 2);
    }

    static void TestInvalidFile() {
        const auto result = Day04::openFile("nonexistent.txt");
        EXPECT_FALSE(result.has_value());
    }

    static void TestAllDirections() {
        const std::string input =
            "....S....\n"
            "S..SA.S..\n"
            ".ASAMA...\n"
            ".SMMM....\n"
            "SAMXMASAM\n"
            "..MMM.A.S\n"
            ".A.AAA..S\n"
            "S..SMSS.S\n"
            "....S....";

        std::vector<char> data;
        constexpr size_t rows = 9;
        constexpr size_t cols = 9;
        for (char c : input) {
            if (c != '\n') data.push_back(c);
        }

        std::vector<size_t> startPositions;
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == 'X') {
                startPositions.push_back(i);
            }
        }

        const auto results = Day04::findAll(data, startPositions, rows, cols);

        // Should find exactly 8 patterns
        EXPECT_EQ(results.size(), 8);

        std::set<size_t> foundDirections;
        for (const auto& [row, col, dir] : results) {
            foundDirections.insert(dir);
        }

        // Each direction should be found
        for (size_t dir = 0; dir < 8; ++dir) {
            EXPECT_TRUE(foundDirections.contains(dir))
                << "Direction " << dir << " not found";
        }
    }

    static void TestEdgeCases() {
        // Test pattern at edge of grid
        const std::string input = "XMAS\nXMAS";
        const auto path = createTempFile(input);
        auto file = Day04::openFile(path);
        ASSERT_TRUE(file.has_value());

        auto [data, rows] = Day04::getDataArray(file.value());
        const auto cols = data.size() / rows;

        const std::vector startPositions{0, cols}; // Both X positions
        const auto results = Day04::findAll(data, startPositions, rows, cols);

        EXPECT_EQ(results.size(), 2); // Should find both horizontal patterns
    }
};

TEST_F(Day04Test, SimpleHorizontalPattern) {
    TestSimpleHorizontalPattern();
}

TEST_F(Day04Test, VerticalPattern) {
    TestVerticalPattern();
}

TEST_F(Day04Test, DiagonalPattern) {
    TestDiagonalPattern();
}

TEST_F(Day04Test, MultiplePatterns) {
    TestMultiplePatterns();
}

TEST_F(Day04Test, NoPattern) {
    TestNoPattern();
}

TEST_F(Day04Test, PartTwoSimplePattern) {
    TestPartTwoSimplePattern();
}

TEST_F(Day04Test, PartTwoMultiplePatterns) {
    TestPartTwoMultiplePatterns();
}

TEST_F(Day04Test, InvalidFile) {
    TestInvalidFile();
}

TEST_F(Day04Test, AllDirections) {
    TestAllDirections();
}

TEST_F(Day04Test, EdgeCases) {
    TestEdgeCases();
}