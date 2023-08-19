#include "gtest/gtest.h"
#include "cubing/ScrambleProcessing.h"
#include "cubing/MovesVector.h"
#include <vector>
#include <string>
#include <unordered_set>

using namespace cubing;

TEST(ScrambleProcessingTests, Basic) {
    const std::vector<std::pair<std::string, std::string>> good_and_worse_pairs = {
        {"R", "R U"},
        {"R B", "R Bw"}, // Bw is worse than B
        {"U R F D'", "U R F D2"}, // double is worse
        {"Rw' S L", "R U R' U'"}, // more moves -> worse
        {"Bw2", "F2 z2"}, // there's no way F2 z2 is better than the equivalent wide move
        {"Fw2", "Bw2"}, // F move should be better
        {"F2", "Fw2"}, // non-wide move should be better
        {"S'", "S2"}, // single move is always better than double
    };
    for (const auto& [good, worse] : good_and_worse_pairs) {
        auto good_score = execution_convenience_score(good);
        auto worse_score = execution_convenience_score(worse);
        ASSERT_LE(good_score, worse_score) << good << ", worse = " << worse;
    }
}

TEST(ScrambleProcessingTests, toStringCombinedMoves) {
    const std::vector<std::pair<std::string, std::string>> unwrapped_and_wrapped = {
        {"F2 S2", "Fw2"},
        {"F S B'", "z"},
        {"F' S' B", "z'"}, // this
        {"F2 S2 B2", "z2"}, // this
    };
    for (const auto& [unwrapped, wrapped] : unwrapped_and_wrapped) {
        auto scramble = MovesVector<sidesAndMid333>::from_string(unwrapped);
        ASSERT_EQ(scramble.to_string_combined_moves(), wrapped);
    }
}
