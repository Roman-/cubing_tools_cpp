#include "gtest/gtest.h"
#include "cubing/CubeState.h"
#include "cubing/CubingDefs.h"

using namespace cubing;
TEST(Cube, Basic) {
    auto doChecks = [](auto cube) {
        ASSERT_TRUE(cube.isSolved());
        ASSERT_TRUE(cube.isCorrectlyOriented());
    };
    doChecks(CubeState<sides333>());
    doChecks(CubeState<sidesAndMid333>());
    doChecks(CubeState<allMoves555>());
}

template<QtmMoveSetSize moveSetSize>
void applyScrambleMoveTests(CubeState<moveSetSize> cube) {
    for (size_t m = 0; m < moveSetSize; ++m) {
        const auto m2 = m + moveSetSize;
        const auto m_prime = m + 2 * moveSetSize;

        // M M'
        ASSERT_TRUE(cube.isSolved());
        cube.applyScrambleMove(m);
        ASSERT_FALSE(cube.isSolved());
        cube.applyScrambleMove(m_prime);
        ASSERT_TRUE(cube.isSolved());

        // M' M
        ASSERT_TRUE(cube.isSolved());
        cube.applyScrambleMove(m_prime);
        ASSERT_FALSE(cube.isSolved());
        cube.applyScrambleMove(m);
        ASSERT_TRUE(cube.isSolved());

        // M' M' M2
        cube.applyScrambleMove(m_prime);
        ASSERT_FALSE(cube.isSolved());
        cube.applyScrambleMove(m_prime);
        ASSERT_FALSE(cube.isSolved());
        cube.applyScrambleMove(m2);
        ASSERT_TRUE(cube.isSolved());

        // M M2 M
        cube.applyScrambleMove(m);
        ASSERT_FALSE(cube.isSolved());
        cube.applyScrambleMove(m2);
        ASSERT_FALSE(cube.isSolved());
        cube.applyScrambleMove(m);
        ASSERT_TRUE(cube.isSolved());
    }
}

TEST(Cube, ApplyScrambleMove) {
    applyScrambleMoveTests(CubeState<sides333>());
    applyScrambleMoveTests(CubeState<sidesAndMid333>());
    applyScrambleMoveTests(CubeState<allMoves555>());
}

template<QtmMoveSetSize moveSetSize>
void applyScrambleStringTests(CubeState<moveSetSize> cube) {
    ASSERT_TRUE(cube.isSolved());
/*
    const std::string sexy = "R U R' U'";
    for (size_t i = 0; i < 6; ++i) {
        if (i != 0) {
            ASSERT_FALSE(cube.isSolved());
        }
        cube.applyScramble(sexy);
    }
*/
cube.applyScramble("R R'");
    ASSERT_TRUE(cube.isSolved()) << moveSetSize << "\n" << cube.toString()
                                 << "\ninitial state:\n" << CubeState<moveSetSize>().toString();
}

TEST(Cube, ApplyScrambleString) {
    applyScrambleStringTests(CubeState<sides333>());
//    applyScrambleStringTests(CubeState<sidesAndMid333>());
//    applyScrambleStringTests(CubeState<allMoves555>());
}

TEST(Cube, SideColors) {
    CubeState<sides333> cube;
    ASSERT_EQ(cube.topSideStickers(true), "WWWWWWWWW");
    ASSERT_EQ(cube.frontSideStickers(true), "GGGGGGGGG");
}
