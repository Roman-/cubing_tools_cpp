#include "gtest/gtest.h"
#include "cubing/MovesVector.h"
#include <vector>
#include <string>

using namespace cubing;

TEST(MovesVector, ToStringSides333) {
    const std::vector<std::string> scrambles = {"R U R'", "R' L", "R U F2 L' D' B2"};
    for (const auto& initial : scrambles) {
        const auto v = MovesVector<sides333>::from_string(initial);
        const std::string result = v.to_string(false);
        ASSERT_EQ(result, initial);
    }
}

TEST(MovesVector, ToStringSidesAndMid333) {
    const std::vector<std::string> scrambles = {"R U R'", "F U R B2 S M", "R' M"};
    for (const auto& initial : scrambles) {
        const auto v = MovesVector<sidesAndMid333>::from_string(initial);
        const std::string result = v.to_string(false);
        ASSERT_EQ(result, initial);
    }
}

TEST(MovesVector, MoveCountSme) {
    std::vector<std::pair<std::string, size_t>> scrambles_and_move_counts = {
        {"R' M", 1}, // Rw'
        {"R U R'", 3},
        {"R U R' M", 3},
        {"R M", 2}, // can't be combined
        {"F B'", 2}, // can't be combined
        {"F S B'", 1}, // combined into z
        {"R M' L' U E' D' F", 3}, // x' y F
        {"F2 S2 B'", 2}, // Fw2 B'
    };

    for (const auto& [scramble, move_count]: scrambles_and_move_counts) {
        const auto v = MovesVector<sidesAndMid333>::from_string(scramble);
        ASSERT_EQ(v.move_count_SME_combined(), move_count) << scramble;
    }
}
