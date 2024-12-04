#include <iostream>
#include "cubing/CubingDefs.h"
#include "cubing/MosaicDefs.h"
#include "cubing/CubeState.h"
#include "cubing/ScrambleProcessing.h"
#include "cubing/IterativeScramble.h"
#include "cubing/Helpers.h"
#include <fmt/format.h>
#include <csignal>
#include <filesystem>

using namespace cubing;

static volatile bool exit_flag = false;
static constexpr QtmMoveSetSize QTM_MOVE_SET_SIZE = QtmMoveSetSize::sidesAndMid333; // change to sides333 if needed
static constexpr size_t NUM_STICKERS = 9; // change to NUM_STICKERS_ON_ONE_SIDE if aiming for 8-sticker mode
static constexpr size_t NUM_COLORS_IN_CUBE = 6;

static const auto now = [] { return std::chrono::steady_clock::now(); };

static IterativeScramble<QTM_MOVE_SET_SIZE> loadScrambleFromFile(const std::string& path) {
    const auto lines = getFileContentsAsLines(path);
    if (lines.size() != 1) {
        std::cout << "Couldn't load scramble from " << path << ", starting from scratch" << std::endl;
        return {};
    }
    const auto moves = MovesVector<QTM_MOVE_SET_SIZE>::from_string(lines.front());
    return IterativeScramble<QTM_MOVE_SET_SIZE>::from_moves(moves);
}

static void saveProgress(const std::string& working_dir, const PatternToAlgAndConvenienceMap& map,
                         const IterativeScramble<QTM_MOVE_SET_SIZE>& scramble) {
    if (!map.save_to_file(fmt::format("{}/{}", working_dir, ALGS_FILE_NAME))) {
        std::cout << "Failed to save algs to " << working_dir << "/" << ALGS_FILE_NAME << std::endl;
        exit(-1);
    }
    saveToFile(fmt::format("{}/{}", working_dir, SCRAMBLE_FILE_NAME), scramble.get().to_string());
}

int main(int argc, char** argv) {
    if (argc < 2 || !std::filesystem::is_directory(argv[1])) {
        std::cerr << "usage: " << argv[0] << " /path/to/working_dir" << std::endl;
        exit(-1);
    }
    const auto working_dir = argv[1];
    auto scramble = loadScrambleFromFile(fmt::format("{}/{}", working_dir, SCRAMBLE_FILE_NAME));
    auto patternToAlgAndConvenience = PatternToAlgAndConvenienceMap::load_from_file(fmt::format("{}/{}", working_dir, ALGS_FILE_NAME));

    for (int sig : {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGABRT}) {
        std::signal(sig, [](int) { exit_flag = true; });
    }

    const size_t totalPatterns = std::pow(NUM_COLORS_IN_CUBE, NUM_STICKERS); // each of 6 colors of the cube must be taken by every sticker
    auto last_hit_made = now();
    std::string latest_found_alg;
    uint64_t counter{0}, num_hits{0};
    while (!exit_flag) {
        CubeState<QTM_MOVE_SET_SIZE> cube;
        cube.applyScramble(scramble.get());
        if (cube.doFrontAndBackSidesHaveSamePatternWithOppositeColors()) {
            const auto pattern = cube.frontSideStickers();
            const auto alg = scramble.get().to_string_combined_moves();
            if (patternToAlgAndConvenience.insert_if_more_convenient(pattern, alg)) {
                ++num_hits;
                last_hit_made = now();
                latest_found_alg = alg;
            }
        }

        ++scramble;
        ++counter;

        if (counter % 1'000'000 == 0) {
            std::cout << (patternToAlgAndConvenience.size() == totalPatterns ? "FOUND ALL " : "Found ")
                      << patternToAlgAndConvenience.size() << " of " << totalPatterns
                      << ", " << scramble.progress() << " | " << num_hits << " hits, last "
                      << std::chrono::duration_cast<std::chrono::seconds>(now() - last_hit_made).count()
                      << "s ago: " << latest_found_alg << std::endl;
        }
        if (counter % 100'000'000 == 0) {
            std::cout << "Saving progress to " << working_dir << "..." << std::endl;
            saveProgress(working_dir, patternToAlgAndConvenience, scramble);
            std::cout << "Saved, resuming search" << std::endl;
        }
    }
    std::cout << "Saving to " << working_dir << " and exiting..." << std::endl;
    saveProgress(working_dir, patternToAlgAndConvenience, scramble);
    std::cout << "Done";
}
