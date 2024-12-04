#include <gtest/gtest.h>

#include "Day03.h"

class Day03Test : public ::testing::Test {
protected:
    static std::filesystem::path createTempFile(const std::string& content) {
        auto path = std::filesystem::temp_directory_path() / "test_input.txt";
        std::ofstream file(path);
        file << content;
        return path;
    }

    static void TestSimpleMultiplication() {
        const std::string input = "mul(2,3)";
        const auto result = Day03::processMultiplications<int64_t>(input);
        EXPECT_EQ(result, 6);
    }

    static void TestMultipleMultiplications() {
        const std::string input = "mul(2,3) text mul(4,5)";
        const auto result = Day03::processMultiplications<int64_t>(input);
        EXPECT_EQ(result, 26); // 2*3 + 4*5 = 6 + 20 = 26
    }

    static void TestInvalidMultiplication() {
        EXPECT_THROW(Day03::stoT<int64_t>("invalid"), aoc::exceptions::DataFormatError);
    }

    static void TestEmptyInput() {
        const std::string input;
        const auto result = Day03::processMultiplications<int64_t>(input);
        EXPECT_EQ(result, 0);
    }

    static void TestDoSection() {
        const std::string input = "do()mul(2,3)don't()";
        const auto result = Day03::sumMultiplicationsDD<int64_t>(input);
        EXPECT_EQ(result, 6);
    }

    static void TestMultipleDoSections() {
        const std::string input = "do()mul(2,3)don't() some text do()mul(4,5)don't()";
        const auto result = Day03::sumMultiplicationsDD<int64_t>(input);
        EXPECT_EQ(result, 26); // 2*3 + 4*5 = 6 + 20 = 26
    }

    static void TestNestedSections() {
        const std::string input = "do()mul(2,3)do()mul(4,5)don't()don't()";
        const auto result = Day03::sumMultiplicationsDD<int64_t>(input);
        EXPECT_EQ(result, 26);
    }

    static void TestFileOperations() {
        const auto path = createTempFile("mul(2,3)\nmul(4,5)");
        const auto file = Day03::openFile(path);
        ASSERT_TRUE(file.has_value());

        std::stringstream buffer;
        buffer << file.value().rdbuf();
        const std::string content = buffer.str();

        const auto result = Day03::processMultiplications<int64_t>(content);
        EXPECT_EQ(result, 26);
    }

    static void TestInvalidFile() {
        const auto result = Day03::openFile("nonexistent.txt");
        EXPECT_FALSE(result.has_value());
    }

    static void TestLargeNumbers() {
        const std::string input = "mul(999,999)";
        const auto result = Day03::processMultiplications<int64_t>(input);
        EXPECT_EQ(result, 998001);
    }

    static void TestOverlappingSections() {
        const std::string input = "do()mul(2,3)don't()do()mul(4,5)don't()";
        const auto result = Day03::sumMultiplicationsDD<int64_t>(input);
        EXPECT_EQ(result, 26);
    }
};

TEST_F(Day03Test, SimpleMultiplication) {
    TestSimpleMultiplication();
}

TEST_F(Day03Test, MultipleMultiplications) {
    TestMultipleMultiplications();
}

TEST_F(Day03Test, InvalidMultiplication) {
    TestInvalidMultiplication();
}

TEST_F(Day03Test, EmptyInput) {
    TestEmptyInput();
}

TEST_F(Day03Test, DoSection) {
    TestDoSection();
}

TEST_F(Day03Test, MultipleDoSections) {
    TestMultipleDoSections();
}

TEST_F(Day03Test, NestedSections) {
    TestNestedSections();
}

TEST_F(Day03Test, FileOperations) {
    TestFileOperations();
}

TEST_F(Day03Test, InvalidFile) {
    TestInvalidFile();
}

TEST_F(Day03Test, LargeNumbers) {
    TestLargeNumbers();
}

TEST_F(Day03Test, OverlappingSections) {
    TestOverlappingSections();
}