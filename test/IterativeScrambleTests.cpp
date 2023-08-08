#include "gtest/gtest.h"
#include "cubing/CubeState.h"
#include "cubing/CubingDefs.h"
#include "cubing/IterativeScramble.h"
#include "cubing/ScrambleProcessing.h"
#include <vector>
#include <string>
#include <unordered_set>

using namespace cubing;

template<QtmMoveSetSize moveSetSize>
void doBasicChecks() {
    IterativeScramble<moveSetSize> scr;
    ASSERT_EQ(scr.size(), 0);
    ++scr;
    ASSERT_EQ(scr.size(), 1);
}

TEST(IterativeScramble, Basic) {
    doBasicChecks<sides333>();
    doBasicChecks<sidesAndMid333>();
    doBasicChecks<allMoves555>();
}

template<QtmMoveSetSize moveSetSize>
std::string bruteForceSolve(const std::string& scrambleToApply) {
    CubeState<moveSetSize> cube;
    uint64_t counter = 0, maxCounter = 10000;
    cube.applyScramble(scrambleToApply);
    IterativeScramble<moveSetSize> iterative_scramble;
    while (++counter < maxCounter) {
        auto cubeCopy = cube;
        cubeCopy.applyScramble(iterative_scramble.get());
        if (cubeCopy.isSolved()) {
            return iterative_scramble.get().to_string();
        }
        ++iterative_scramble;
    }
    return {};
}

TEST(IterativeScramble, BruteForceSolve) {
    const std::vector<std::string> easyScrambles = {"", "R", "R U"};
    for (const auto& scramble : easyScrambles) {
        const auto solution = invertScramble(scramble);
        ASSERT_EQ(bruteForceSolve<sides333>(scramble), solution) << solution;
        ASSERT_EQ(bruteForceSolve<sidesAndMid333>(scramble), solution) << solution;
        ASSERT_EQ(bruteForceSolve<allMoves555>(scramble), solution) << solution;
    }

    ASSERT_EQ(bruteForceSolve<allMoves555>("b M"), "M' b'");
}

template<QtmMoveSetSize moveSetSize>
void testToString() {
    IterativeScramble<moveSetSize> scramble;
    uint64_t iterations_made{0};
    while(scramble.size() < 3) {
        const auto str = scramble.get().to_string();
        std::for_each(str.begin(), str.end(), [&](char c) {
            if (c == '?') {
                std::cout << "MOVES:\n";
                for (size_t i = 0; i < scramble.size(); ++i) {
                    std::cout << "[" << i << "]: " << int(scramble.get()[i]) << '\n';
                }

                ASSERT_FALSE(true) << moveSetSize << ", itrs made = " << iterations_made << ", str=" << str;
            }
        });
        ++scramble, ++iterations_made;
    }
}

TEST(IterativeScramble, ToString) {
    testToString<sides333>();
    testToString<sidesAndMid333>();
    testToString<allMoves555>();
}

TEST(IterativeScramble, ParallelLayerMoves) {
    struct Group {
        std::unordered_set<std::string> equivalent_algs; // {R2 M} / {M R2} / {R R M} / {R M R} / {R R M}
        size_t num_matches{0};
        void check(const std::string& alg) {
            if (equivalent_algs.count(alg)) {
                ++num_matches;
            }
        }
    };
    std::vector<Group> groups = {
        {.equivalent_algs = {"R2 M", "M R2", "M R R", "R M R", "R R M"}},
        {.equivalent_algs = {"S' F'", "F' S'"}},
        {.equivalent_algs = {"M2 R2 L2", "M2 L2 R2", "R2 M2 L2", "R2 L2 M2", "L2 M2 R2", "L2 R2 M2"}},
    };
    std::unordered_set<std::string> existing_algs{"M2 F", "S' R U", "S' R U'", "M2 S2 E2"};

    IterativeScramble<sidesAndMid333> scramble;
    while (scramble.size() <= 3) {
        ++scramble;
        const std::string alg = scramble.get().to_string();
        for (auto& g : groups) {
            g.check(alg);
            existing_algs.erase(alg);
        }
    }
    // make sure that each group has exactly 1 alg checked
    for (const auto& g : groups) {
        ASSERT_EQ(g.num_matches, 1) << *g.equivalent_algs.begin();
    }
    ASSERT_TRUE(existing_algs.empty()) << *existing_algs.begin();
}
