#include "Mosaic.h"

#include <iostream>
#include <fstream>
#include <strutil.h>
#include <unordered_map>
#include <fmt/format.h>
#include <unordered_set>
#include <csignal>
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

static volatile bool exit_flag = false;

void doMosaicMesTest(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " /path/to/output.txt" << std::endl;
        exit(-1);
    }
    std::ofstream file(argv[1]);
    if (!file.is_open()) {
        throw std::runtime_error(fmt::format("failed to open file {}", argv[1]));
    }
    std::signal(SIGINT, [](int) { exit_flag = true; });
    std::signal(SIGTERM, [](int) { exit_flag = true; });

    IterativeScramble<sidesAndMid333> scramble;
    std::unordered_map<std::string, MovesVector<sidesAndMid333>> patternToAlg;
    const size_t totalPatterns = std::pow(6, 9); // 6 colors of the cube must be taken by all of 9 stickers
    uint64_t counter{0};
    while (!exit_flag) {
        const bool found_all_patterns = patternToAlg.size() == totalPatterns;
        if (++counter % 50000 == 0) {
            std::cout << (found_all_patterns ? "FOUND ALL " : "found ") << patternToAlg.size() << " of " << totalPatterns
                << ", " << scramble.progress() << std::endl;
        }
        CubeState<sidesAndMid333> cube;
        cube.applyScramble(scramble.get());
        const auto pattern = cube.topSideStickers();
        const auto itr = patternToAlg.find(pattern);
        if (itr == patternToAlg.end() || itr->second.move_count_combined() > scramble.get().move_count_combined()) {
            patternToAlg[pattern] = scramble.get();
        }
        ++scramble;
    }
    std::cout << "Exiting and saving..." << std::endl;
    for (const auto& [pattern, alg] : patternToAlg) {
        file << pattern << "\t" << alg.to_string() << '\n';
    }
    std::cout << "Saved map to " << argv[1] << std::endl;
}

}