#include "gtest/gtest.h"
#include "cubing/ScrambleProcessing.h"
#include <vector>
#include <string>
#include <unordered_set>

using namespace cubing;

TEST(ConvenienceScore, Basic) {
    const std::vector<std::pair<std::string, std::string>> good_and_worse_pairs = {
        {"R", "R U"}, // less moves
        {"R B", "R Bw"}, // Bw is worse than B
        {"U R F D'", "U R F D2"}, // double is worse
        {"Rw' S L", "R U R' U'"}, // more moves -> worse
    };
    for (const auto& [good, worse] : good_and_worse_pairs) {
        auto good_score = execution_convenience_score(good);
        auto worse_score = execution_convenience_score(worse);
        ASSERT_LE(good_score, worse_score) << good << ", worse = " << worse;
    }
}