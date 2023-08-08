#include "Mosaic.h"

#include <iostream>
#include <fstream>
#include <strutil.h>
#include <unordered_map>
#include <fmt/format.h>
#include <unordered_set>
#include <csignal>
#include <chrono>
#include "ScrambleProcessing.h"
#include "IterativeScramble.h"
#include "CubeState.h"
#include "Helpers.h"

namespace cubing {

static volatile bool exit_flag = false;
auto now = [] {
    return std::chrono::steady_clock::now();
};

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

    const size_t totalPatterns = std::pow(6, 9); // each of 6 colors of the cube must be taken by all of 9 stickers
    auto last_hit_made = now();
    uint64_t counter{0}, num_hits{0};
    while (!exit_flag) {
        CubeState<sidesAndMid333> cube;
        cube.applyScramble(scramble.get());
        const auto pattern = cube.topSideStickers();
        const auto itr = patternToAlg.find(pattern);
        if (itr == patternToAlg.end() || itr->second.move_count_combined() > scramble.get().move_count_combined()) {
            patternToAlg[pattern] = scramble.get();
            ++num_hits;
            last_hit_made = now();
        }
        ++scramble;

        if (++counter % 1'000'000 == 0) {
            std::cout << (patternToAlg.size() == totalPatterns ? "FOUND ALL " : "Found ")
                      << patternToAlg.size() << " of " << totalPatterns
                      << ", " << scramble.progress() << " | " << num_hits << " hits, last hit: "
                      << std::chrono::duration_cast<std::chrono::seconds>(now() - last_hit_made).count()
                      << "s ago" << std::endl;
        }

    }
    std::cout << "Exiting and saving..." << std::endl;
    for (const auto& [pattern, alg] : patternToAlg) {
        file << pattern << "\t" << alg.to_string() << '\n';
    }
    std::cout << "Saved map to " << argv[1] << std::endl;
}

}