#include "Day01.h"
#include "Day02.h"
#include "Day03.h"
#include "Day04.h"
#include "Day05.h"

int main() {
    std::println("Day 1:");
    Day01::partOne();
    Day01::partTwo();
    std::println("Day 2:");
    Day02::partOne();
    Day02::partTwoBruteForce();
    Day02::partTwoSmart();
    std::println("Day 3:");
    Day03::partOne();
    Day03::partTwo();
    std::println("Day 4:");
    Day04::partOne();
    Day04::partTwo();
    std::println("Day 5:");
    Day05::partOne();
    Day05::partTwo();
    return 0;
}
