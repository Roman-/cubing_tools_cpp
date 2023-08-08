#include "Mosaic.h"

#include <iostream>
#include <fstream>
#include <strutil.h>
#include <unordered_map>
#include <fmt/format.h>
#include <unordered_set>
#include <csignal>
#include <filesystem>
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

struct AlgForPattern {
    std::string alg; // wide moves and rotations allowed
    uint32_t convenience_score; // lower is better
};

using PatternToAlgMap = std::unordered_map<std::string, AlgForPattern>;
static const std::string algs_file_name = "algs.txt", scramble_file_name = "scramble.txt";

static std::pair<PatternToAlgMap, IterativeScramble<sidesAndMid333>> load_from_file(const std::string& working_dir) {
    const auto path_to_algs = fmt::format("{}/{}", working_dir, algs_file_name);
    const auto path_to_scr = fmt::format("{}/{}", working_dir, scramble_file_name);
    // if file does not exist or empty, return empty map
    if (!std::filesystem::exists(path_to_algs) || std::filesystem::file_size(path_to_algs) == 0
        || !std::filesystem::exists(path_to_scr) || std::filesystem::file_size(path_to_scr) == 0) {
        auto assert_open_and_clean_file = [] (const std::string& path) {
            std::ofstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error(fmt::format("failed to open file {}", path));
            }
            file.close();
        };
        assert_open_and_clean_file(path_to_algs);
        assert_open_and_clean_file(path_to_scr);
        std::cout << "Generating from scratch" << std::endl;
        return {};
    }
    std::cout << "Loading progress from files..." << std::endl;
    PatternToAlgMap map;
    // load algs line-by-line
    std::ifstream alg_file(path_to_algs);
    if (!alg_file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open the file {}", path_to_algs));
    }
    std::string line;
    while (std::getline(alg_file, line)) {
        const auto num_stickers = 9;
        if (line.size() <= num_stickers || line[num_stickers] != '\t') {
            continue;
        }
        const auto stickers = line.substr(0, num_stickers);
        const auto alg = line.substr(num_stickers + 1);
        map.insert({stickers,
                    {
                        .alg = alg,
                        .convenience_score = execution_convenience_score(alg)
                    }});
    }
    alg_file.close();
    std::cout << "Loaded " << map.size() << " algs" << std::endl;

    const auto scr_lines = getFileContentsAsLines(path_to_scr);
    if (scr_lines.size() != 1) {
        throw std::runtime_error(fmt::format("Expected one line in the file {}, got {}", path_to_scr, scr_lines.size()));
    }
    const auto moves = MovesVector<sidesAndMid333>::from_string(scr_lines.front());
    auto scr = IterativeScramble<sidesAndMid333>::from_moves(moves);
    return {map, scr};
}

void save_progress(const PatternToAlgMap& map, const IterativeScramble<sidesAndMid333>& scr,
                   const std::string& working_dir) {
    const auto path_to_algs = fmt::format("{}/{}", working_dir, algs_file_name);
    const auto path_to_scr = fmt::format("{}/{}", working_dir, scramble_file_name);
    std::ofstream alg_file(path_to_algs);
    if (!alg_file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open the file {}", path_to_algs));
    }
    for (const auto& [pattern, alg] : map) {
        alg_file << pattern << '\t' << alg.alg << '\n';
    }
    alg_file.close();
    std::ofstream scr_file(path_to_scr);
    if (!scr_file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open the file {}", path_to_scr));
    }
    scr_file << scr.get().to_string();
    scr_file.close();
}

void doMosaicMesTest(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " /path/to/working_dir" << std::endl;
        exit(-1);
    }
    const auto working_dir = argv[1];
    auto [patternToAlg, scramble] = load_from_file(working_dir);

    std::signal(SIGINT, [](int) { exit_flag = true; });
    std::signal(SIGTERM, [](int) { exit_flag = true; });
    std::signal(SIGHUP, [](int) { exit_flag = true; });

    const size_t totalPatterns = std::pow(6, 9); // each of 6 colors of the cube must be taken by all of 9 stickers
    auto last_hit_made = now();
    uint64_t counter{0}, num_hits{0};
    while (!exit_flag) {
        CubeState<sidesAndMid333> cube;
        cube.applyScramble(scramble.get());
        const auto pattern = cube.topSideStickers();
        const auto itr = patternToAlg.find(pattern);
        const auto alg_string = scramble.get().to_string(); // TODO to_string_combine_moves
        const auto num_moves = uint32_t(std::count(alg_string.begin(), alg_string.end(), ' ')) + 1;
        const auto convenience_score = execution_convenience_score(alg_string);
        if (itr == patternToAlg.end() || itr->second.convenience_score > convenience_score) {
            patternToAlg[pattern] = {
                .alg = alg_string,
                .convenience_score = convenience_score
            };
            ++num_hits;
            last_hit_made = now();
        }
        ++scramble;
        ++counter;

        if (counter % 1'000'000 == 0) {
            std::cout << (patternToAlg.size() == totalPatterns ? "FOUND ALL " : "Found ")
                      << patternToAlg.size() << " of " << totalPatterns
                      << ", " << scramble.progress() << " | " << num_hits << " hits, last hit: "
                      << std::chrono::duration_cast<std::chrono::seconds>(now() - last_hit_made).count()
                      << "s ago" << std::endl;
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

}