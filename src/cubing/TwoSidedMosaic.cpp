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

using PatternToAlgMap = std::unordered_map<std::string, std::string>;
static const std::string algs_file_name = "algs.txt", scramble_file_name = "scramble.txt";

static std::pair<PatternToAlgMap, IterativeScramble<sides333>> load_from_file(const std::string& working_dir) {
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
        map.insert({stickers, alg});
    }
    alg_file.close();
    std::cout << "Loaded " << map.size() << " algs" << std::endl;

    const auto scr_lines = getFileContentsAsLines(path_to_scr);
    if (scr_lines.size() != 1) {
        throw std::runtime_error(fmt::format("Expected one line in the file {}, got {}", path_to_scr, scr_lines.size()));
    }
    const auto moves = MovesVector<sides333>::from_string(scr_lines.front());
    auto scr = IterativeScramble<sides333>::from_moves(moves);
    return {map, scr};
}

static void save_progress(const PatternToAlgMap& map, const IterativeScramble<sides333>& scr,
                   const std::string& working_dir) {
    const auto path_to_algs = fmt::format("{}/{}", working_dir, algs_file_name);
    const auto path_to_scr = fmt::format("{}/{}", working_dir, scramble_file_name);
    std::ofstream alg_file(path_to_algs);
    if (!alg_file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open the file {}", path_to_algs));
    }
    for (const auto& [pattern, alg] : map) {
        alg_file << pattern << '\t' << alg << '\n';
    }
    alg_file.close();
    std::ofstream scr_file(path_to_scr);
    if (!scr_file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open the file {}", path_to_scr));
    }
    scr_file << scr.get().to_string();
    scr_file.close();
}

static void save_pace(const std::string& working_dir, size_t num_algs) {
    const auto path_to_pace = fmt::format("{}/pace.tsv", working_dir, algs_file_name);
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
    auto [patternToAlg, scramble] = load_from_file(working_dir);

    std::signal(SIGINT, [](int) { exit_flag = true; });
    std::signal(SIGTERM, [](int) { exit_flag = true; });
    std::signal(SIGHUP, [](int) { exit_flag = true; });

    const size_t totalPatterns = std::pow(6, 8); // each of 6 colors of the cube must be taken by all of 8 stickers around the center
    auto last_hit_made = now();
    std::string latest_found_alg;
    uint64_t counter{0}, num_hits{0};
    while (!exit_flag) {
        CubeState<sides333> cube;
        cube.applyScramble(scramble.get());
        if (cube.doFrontAndBackSidesHaveSamePatternWithOppositeColors()) {
            const auto pattern = cube.frontSideStickers();
            const auto itr = patternToAlg.find(pattern);
            if (itr == patternToAlg.end()) {
                patternToAlg[pattern] = latest_found_alg = scramble.get().to_string();
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
            save_pace(working_dir, patternToAlg.size());
            std::cout << "Saved, resuming search" << std::endl;
        }
    }
    std::cout << "Saving to " << working_dir << " and exiting..." << std::endl;
    save_progress(patternToAlg, scramble, working_dir);
    std::cout << "Done";
}

void doSymmetrySubstitutionThing(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " /path/to/working_dir" << std::endl;
        exit(-1);
    }
    const auto working_dir = argv[1];
    auto [patternToAlg, _] = load_from_file(working_dir);
    auto augmented_map = patternToAlg;

    // add in map if it's not there yet
    const auto check_alg = [&](const std::string& alg) {
        CubeState<sides333> cube;
        cube.applyScramble(alg);
        if (!cube.doFrontAndBackSidesHaveSamePatternWithOppositeColors()) {
            return;
        }
        const auto pattern = cube.frontSideStickers();
        const auto itr = augmented_map.find(pattern);
        if (itr == augmented_map.end() || itr->second.size() > alg.size()) {
            augmented_map.insert({pattern, alg});
        }
    };

    const auto explore_alg = [&](const std::string& alg) {
        check_alg(alg);
        check_alg(invertScramble(alg));

        check_alg(left2right(alg));
        check_alg(left2right(invertScramble(alg)));

        check_alg(front2back(alg));
        check_alg(front2back(invertScramble(alg)));

        check_alg(front2back(left2right(alg)));
        check_alg(front2back(left2right(invertScramble(alg))));

        check_alg(left2right(front2back(alg)));
        check_alg(left2right(front2back(invertScramble(alg))));
    };

    const std::string meme = "R' L U D' F B' R' L";
    const std::string centers_shift = "R L' F2 B2 R L' U2 D2";
    // may be used both as prefixes and suffixes, or both
    static const std::vector<std::string> addon_algs = {
        "U2 D2", "R2 L2", "F2 B2",
        "R2 L2 U2 D2", "R2 L2 F2 B2", "U2 D2 F2 B2",
        "R2 L2 U2 D2 F2 B2",
        centers_shift,
        front2back(centers_shift),
        left2right(centers_shift),
        left2right(front2back(centers_shift)),

        meme,
        invertScramble(meme),
        front2back(meme),
        left2right(meme),
        left2right(invertScramble(meme)),
        front2back(invertScramble(meme)),

        // Rotate the result (F2 B2 are already in the map). Then you may recursively explore the resulting map
        "F B'", "F' B", "U D'", "U' D",

        // these do not preserve symmetry
        "R' L F2 B2 R L'",
        "F B' R' L U D",
        "R2", "L2", "F2", "B2", "U2", "D2", // single side moves
        "R' L", "R L'", "F B'", "F' B", "U D'", "U' D",

        // columns - almost full symmetry
        "F2 B2 R2 F2 B2 R2",
        "F2 B2 U2 F2 B2 U2",
        "U2 D2 R2 U2 D2 R2",
        "U2 D2 F2 U2 D2 F2",
        "R2 L2 F2 R2 L2 F2",
        "R2 L2 U2 R2 L2 U2",

        // crazy patterns that almost preserve symmetry
        "R L' F' B D U'",
        invertScramble("R L' F' B D U'"),
        left2right("R L' F' B D U'"),
        left2right(invertScramble("R L' F' B D U'")),
        front2back("R L' F' B D U'"),
        front2back(invertScramble("R L' F' B D U'")),
        front2back(left2right("R L' F' B D U'")),
        front2back(left2right(invertScramble("R L' F' B D U'"))),

        "R2 L2 U2 R2 L2",
        "R2 L2 D2 R2 L2",
        "R2 L2 U2 R2 L2 D2",
        "R2 L2 U2 R2 L2",
        "R2 L2 D2 R2 L2",
        "U2 D2 L2 U2 D2 R2",
        "F2 B2 R2 F2 B2 L2",
        "L R' U2 D2 L' R U2 D2 R2 L2",
        "U2 R2 F2 U2 D2 F2 L2 U2",
        "R2 D2 F2 R2 L2 F2 U2 R2",
        "R2 D2 R2 U2 R2 F2 U2 D2 F2 U2",
        "R2 L' D F2 R' D' R' L U' D R D B2 R' U D2",
        invertScramble("R2 L' D F2 R' D' R' L U' D R D B2 R' U D2"),
        "R D R F R' F' B D R' U' B' U D2",
        invertScramble("R D R F R' F' B D R' U' B' U D2"),
        left2right("R D R F R' F' B D R' U' B' U D2"),
        front2back("R D R F R' F' B D R' U' B' U D2"),

        // other stuff
        "F2 B2 R2 L2 U2",
        "F2 B2 R2 L2 D2",

        "R2 U2 D2 L2",
        "R2 U2 D2 R2 F2 B2",
        "R2 F2 B2 R2 U2 D2",
        "F2 U2 D2 F2 R2 L2",

        "U2 R2 L2 U2",
        "L2 U2 D2 L2",
        "F2 U2 D2 F2",
        "F2 R2 L2 F2",

        // (R2 U2)*3
        "R2 U2 R2 U2 R2 U2",
        "L2 U2 L2 U2 L2 U2",
        "R2 D2 R2 D2 R2 D2",
        "L2 D2 L2 D2 L2 D2",
    };

    uint64_t num_algs_checked = 0;
    for (const auto& [pattern, alg] : patternToAlg) {
        if (++num_algs_checked % 1000 == 0) {
            const auto all_algs_found_notice = (augmented_map.size() == 1679616) ? "ALL ALGS FOUND! " : "";
            std::cout << all_algs_found_notice << "Checked " << num_algs_checked << "/" << patternToAlg.size() << "; new algs added: "
                      << (augmented_map.size() - patternToAlg.size()) << std::endl;
        }
        if (alg.empty()) {
            continue;
        }

        for (const auto& addon : addon_algs) {
            explore_alg(alg);

            explore_alg(fmt::format("{} {}", alg, addon));
            explore_alg(fmt::format("{} {}", addon, alg));
            explore_alg(fmt::format("{} {} {}", addon, alg, addon));

            // transform the alg independently
            explore_alg(fmt::format("{} {}", alg, addon));
            explore_alg(fmt::format("{} {}", invertScramble(alg), addon));
            explore_alg(fmt::format("{} {}", left2right(alg), addon));
            explore_alg(fmt::format("{} {}", left2right(invertScramble(alg)), addon));

            explore_alg(fmt::format("{} {}", addon, alg));
            explore_alg(fmt::format("{} {}", addon, invertScramble(alg)));
            explore_alg(fmt::format("{} {}", addon, left2right(alg)));
            explore_alg(fmt::format("{} {}", addon, left2right(invertScramble(alg))));

            explore_alg(fmt::format("{} {} {}", addon, alg, addon));
            explore_alg(fmt::format("{} {} {}", addon, invertScramble(alg), addon));
            explore_alg(fmt::format("{} {} {}", addon, left2right(alg), addon));
            explore_alg(fmt::format("{} {} {}", addon, left2right(invertScramble(alg)), addon));

            // most random stuff
            explore_alg(fmt::format("{} {}", alg, alg));
            explore_alg(fmt::format("{} {}", alg, invertScramble(left2right(alg))));
            explore_alg(fmt::format("{} {}", alg, invertScramble(front2back(alg))));

            // misc
            explore_alg(fmt::format("{} R2 L2 {}", addon, alg));
            explore_alg(fmt::format("{} U2 D2 {}", addon, alg));
            explore_alg(fmt::format("R2 L2 {} {}", addon, alg));
            explore_alg(fmt::format("U2 D2 {} {}", addon, alg));
            explore_alg(fmt::format("{} {} R2 L2", addon, alg));
            explore_alg(fmt::format("{} {} U2 D2", addon, alg));
            explore_alg(fmt::format("{} R2 L2 {} {}", addon, alg, addon));
            explore_alg(fmt::format("{} U2 D2 {} {}", addon, alg, addon));
        }
    }

    // save
    const auto path_to_augmented_algs = fmt::format("{}/augmented_algs.txt", working_dir);
    std::cout << "Done, saving " << augmented_map.size() << " algs to " << path_to_augmented_algs << std::endl;
    std::ofstream alg_file(path_to_augmented_algs);
    if (!alg_file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open the file {}", path_to_augmented_algs));
    }
    for (const auto& [pattern, alg] : augmented_map) {
        alg_file << pattern << '\t' << alg << '\n';
    }
}

}