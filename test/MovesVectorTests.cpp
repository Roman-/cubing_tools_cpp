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

TEST(MovesVector, CombineMoves) {
    std::vector<std::pair<std::string, std::string>> algs_and_their_combined_versions = {
        {"R U R'", "R U R'"},
        {"R M", "R M"}, // can't be combined
        {"F B'", "F B'"}, // can't be combined
/*
        {"R' M", "Rw'"},
        {"R U M R'", "R U Rw'"},
        {"F S B'", "z"},
        {"R M' L' U E' D' F", "x' y F"},
        {"F2 S2 B'", "Fw2 B'"},
        {"R' M R'", "Rw' R'"},
*/
    };

    for (const auto& [scramble, combined]: algs_and_their_combined_versions) {
        const auto v = MovesVector<sidesAndMid333>::from_string(scramble);
        ASSERT_EQ(v.to_string_combined_moves(), combined) << scramble;
    }
}
