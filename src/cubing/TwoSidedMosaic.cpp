#include "Mosaic.h"
#include "MosaicDefs.h"

#include <iostream>
#include <fstream>
#include <strutil.h>
#include <unordered_map>
#include <fmt/format.h>
#include <csignal>
#include <filesystem>
#include <chrono>
#include "ScrambleProcessing.h"
#include "IterativeScramble.h"
#include "CubeState.h"
#include "Helpers.h"

namespace cubing {

static volatile bool exit_flag = false;

constexpr const std::string_view algs_file_name = "algs.txt", scramble_file_name = "scramble.txt";

static IterativeScramble<sides333> loadScrambleFromFile(const std::string& path) {
    const auto lines = getFileContentsAsLines(path);
    if (lines.size() != 1) {
        throw std::runtime_error(fmt::format("Expected one line in the file {}, got {}", path, lines.size()));
    }
    const auto moves = MovesVector<sides333>::from_string(lines.front());
    return IterativeScramble<sides333>::from_moves(moves);
}

/*
static void save_pace(const std::string& working_dir, size_t num_algs) {
    const auto path_to_pace = fmt::format("{}/pace.tsv", working_dir);
    std::ofstream pace_file;
    pace_file.open(path_to_pace, std::ios_base::app);
    if (!pace_file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open the file {}", path_to_pace));
    }
    pace_file << std::chrono::seconds(std::time(NULL)).count() << '\t' << num_algs << '\n';
    pace_file.close();
}

void doTwoSidedMosaicTest(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " /path/to/working_dir" << std::endl;
        exit(-1);
    }
    const auto working_dir = argv[1];
    auto scramble = loadScrambleFromFile(fmt::format("{}/{}", working_dir, scramble_file_name));
    auto patternToAlg = PatternToAlgMap::load_from_file(fmt::format("{}/{}", working_dir, algs_file_name));

    for (int sig : {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGABRT}) {
        std::signal(sig, [](int) { exit_flag = true; });
    }

    const size_t totalPatterns = std::pow(6, 8); // each of 6 colors of the cube must be taken by all of 8 stickers around the center
    auto last_hit_made = now();
    std::string latest_found_alg;
    uint64_t counter{0}, num_hits{0};
    while (!exit_flag) {
        CubeState<sides333> cube;
        cube.applyScramble(scramble.get());
        if (cube.doFrontAndBackSidesHaveSamePatternWithOppositeColors()) {
            const auto pattern = cube.frontSideStickers();
            if (!patternToAlg.exists(pattern)) {
                patternToAlg.insert(pattern, latest_found_alg = scramble.get().to_string());
                ++num_hits;
                last_hit_made = now();
            }
        }

        ++scramble;
        ++counter;

        if (counter % 1'000'000 == 0) {
            std::cout << (patternToAlg.size() == totalPatterns ? "FOUND ALL " : "Found ")
                      << patternToAlg.size() << " of " << totalPatterns
                      << ", " << scramble.progress() << " | " << num_hits << " hits, last "
                      << std::chrono::duration_cast<std::chrono::seconds>(now() - last_hit_made).count()
                      << "s ago: " << latest_found_alg << std::endl;
        }
        if (counter % 100'000'000 == 0) {
            std::cout << "Saving progress to " << working_dir << "..." << std::endl;
            save_progress(patternToAlg, scramble, working_dir);
            std::cout << "Saved, resuming search" << std::endl;
        }
    }
    std::cout << "Saving to " << working_dir << " and exiting..." << std::endl;
    save_progress(patternToAlg, scramble, working_dir);
    std::cout << "Done";
}
*/

}