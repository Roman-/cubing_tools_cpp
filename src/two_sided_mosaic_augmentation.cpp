#include <iostream>
#include "cubing/CubingDefs.h"
#include "cubing/MosaicDefs.h"
#include "cubing/CubeState.h"
#include "cubing/ScrambleProcessing.h"
#include <fmt/format.h>

using namespace cubing;

/*
 * Having 8-sticker 2-sided mosaic algs map (OYYWGGRRB \t  D F R' F U F D L' F L B2), augment it with new algs to
 * hopefully find algs for all 6^8 = 1'679'616 patterns.
 * */

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " /path/to/algs.txt" << std::endl;
        exit(-1);
    }
    const auto map = PatternToAlgMap::load_from_file(argv[1]);
    if (map.empty()) {
        std::cerr << "No algs found in " << argv[1] << '\n';
        return -1;
    }
    auto augmented_map = map;
    const auto path_to_augmented_algs = fmt::format("{}.augmented.txt", argv[1]);
    auto save_augmented_algs = [&]() {
        if (augmented_map.save_to_file(path_to_augmented_algs)) {
            std::cout << "Saved " << augmented_map.size() << " augmented algs to " << path_to_augmented_algs << '\n';
        } else {
            std::cerr << "Failed to save augmented algs to " << path_to_augmented_algs << '\n';
            exit(-1);
        }
    };
    save_augmented_algs(); // check that file is writable

    // add in map if it's not there yet
    const auto check_alg = [&](const std::string& alg) {
        CubeState<sides333> cube;
        cube.applyScramble(alg);
        if (cube.doFrontAndBackSidesHaveSamePatternWithOppositeColors()) {
            augmented_map.insert_if_shorter(cube.frontSideStickers(), alg);
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
    for (const auto& [pattern, alg] : map.get()) {
        if (++num_algs_checked % 100 == 0) {
            const auto all_algs_found_notice = (augmented_map.size() == 1679616) ? "ALL ALGS FOUND! " : "";
            std::cout << all_algs_found_notice << "Checked " << num_algs_checked << "/" << map.size() << "; new algs added: "
                      << (augmented_map.size() - map.size()) << std::endl;
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
        if (num_algs_checked % 1'000 == 0) {
            save_augmented_algs();
        }
    }
    save_augmented_algs();
    return 0;
}
