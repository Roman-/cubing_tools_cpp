#include "Mosaic.h"

#include <iostream>
#include <fstream>
#include <strutil.h>
#include <unordered_map>
#include <fmt/format.h>
#include <unordered_set>
#include "ScrambleProcessing.h"
#include "IterativeScramble.h"
#include "CubeState.h"
#include "Helpers.h"

namespace cubing {

void doMosaicTest(int argc, char** argv) {
    IterativeScramble<sides333> scramble;
    std::unordered_map<std::string, std::string> patternToAlg;
    const size_t totalPatterns = std::pow(8, 6);
    uint64_t counter{0};
    while (patternToAlg.size() != totalPatterns) {
        if (++counter % 100000 == 0) {
            std::cout << "found " << patternToAlg.size() << " of " << totalPatterns << ", " << scramble.progress() << std::endl;
        }
        CubeState<sides333> cube;
        cube.applyScramble(scramble.get());
        const auto pattern = cube.topSideStickers();
        const auto itr = patternToAlg.find(pattern);
        if (itr == patternToAlg.end()) {
            patternToAlg[pattern] = scramble.get().to_string();
        }
        ++scramble;
    }
}

void doMosaicMesTest(int argc, char** argv) {
    IterativeScramble<sidesAndMid333> scramble;
    std::unordered_map<std::string, std::string> patternToAlg;
    const size_t totalPatterns = std::pow(9, 6); // 9 squares, each takes every of the 6 colors
    uint64_t counter{0};
    while (scramble.size() < 4) {
        if (++counter % 100000 == 0) {
            std::cout << "found " << patternToAlg.size() << " of " << totalPatterns
                << ", " << scramble.progress() << std::endl;
        }
        CubeState<sidesAndMid333> cube;
        cube.applyScramble(scramble.get());
        const auto pattern = cube.topSideStickers();
        const auto itr = patternToAlg.find(pattern);
        if (itr == patternToAlg.end()) {
            patternToAlg[pattern] = scramble.get().to_string();
        }
        ++scramble;
    }
}

}