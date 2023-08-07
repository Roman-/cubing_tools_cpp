/*
#pragma once

#include <iostream>
#include <set>
#include <fstream>
#include <strutil.h>
#include <unordered_map>
#include <fmt/format.h>
#include <random>
#include <unordered_set>
#include "ScrambleProcessing.h"
#include "CubeState.h"
#include "Helpers.h"

using namespace cubing;

/// \param path - path to txt file containing lines. Each line is "key \t setup"
/// \returns map <cube state -> key>, with 4 states for each
using CubeStateToKey = std::unordered_map<std::string, std::string>;
CubeStateToKey loadZbllCasesFromFile(const std::string& path) {
    const auto lines = getFileContentsAsLines(path);
    CubeStateToKey result;
    for (const auto& line : lines) {
        const auto parts = strutil::split(line, '\t');
        if (parts.size() != 2) {
            throw std::runtime_error(fmt::format("loadZbllCasesFromFile: invalid line <{}>", line));
        }
        const auto [key, alg] = std::tuple{parts[0], parts[1]};
        const auto scramble = invertScramble(alg);
        // add 4 variations
        for (const std::string& uMoveSetup : {"", "U ", "U' ", "U2 "}) {
            for (const std::string& auf : {"", " U", " U'", " U2"}) {
                const auto setupWithAuf = fmt::format("{}{}{}", uMoveSetup, scramble, auf);
                CubeState cube(3);
                cube.applyScramble(setupWithAuf);
                const auto state = cube.toString();
*/
/*
                const auto itr = result.find(state);
                if (itr != result.end() && itr->second != key) {
                    throw std::runtime_error(
                        fmt::format("last time, state {} was key {}, this time - key {}. Alg is <{}>", state, itr->second,
                                    key, setupWithAuf));
                }
*//*

                result[state] = key;
            }
        }
    }
    return result;
}

using KeyToScramblesMap = std::unordered_map<std::string, std::vector<std::string>>;
using ScramblesSet = std::unordered_set<std::string>;

/// load scrambles in rock notation from file and populate them (left-to-right, inverse etc.)
ScramblesSet loadRockScrambles(const std::string& dirPath, size_t numMoves,
                               bool includeAlgsStartingOrEndingWithU = true) {
    const auto path = fmt::format("{}/firsttwo-{}.txt", dirPath, numMoves);
    ScramblesSet result;
    const auto lines = getFileContentsAsLines(path);
    KeyToScramblesMap keyToScrambles;
    auto insertScrambles = [&](const std::vector<std::string>& scrambles) {
        for (const auto& s : scrambles) {
            if (includeAlgsStartingOrEndingWithU || (s.front() != 'U' && s.back() != 'U' && s[s.size() - 2] != 'U')) {
                result.insert(s);
            }
        }
    };

    uint64_t counter{0};
    for (const auto& rock_scramble : lines) {
        const auto scramble = fromRokickiNotation(rock_scramble);
        const auto inverted = invertScramble(scramble);
        const auto l2r = left2right(scramble);
        const auto invertedL2r = left2right(inverted);
        const auto f2b = front2back(scramble);
        const auto invertedF2b = front2back(inverted);
        const auto f2bl2r = left2right(f2b);
        const auto f2bl2rInverted = invertScramble(f2bl2r);

        insertScrambles({scramble, inverted, l2r, invertedL2r, f2b, invertedF2b, f2bl2r, f2bl2rInverted});
        if (++counter % 10000 == 0) {
            std::cout << "loadRockScrambles: processed " << counter << "/" << lines.size() << std::endl;
        }
    }
    return result;
}

KeyToScramblesMap processScrambles(const std::unordered_map<std::string, std::string>& zbll_cases_map,
                                   const ScramblesSet& scrambles) {
    KeyToScramblesMap keyToScrambles;
    uint64_t counter{0};
    for (const auto& scramble : scrambles) {
        // which zbll key does it correspond to ? No rotation requires, only the U auf
        CubeState cube(3);
        cube.applyScramble(scramble);
        const auto itr = zbll_cases_map.find(cube.toString());
        if (itr != zbll_cases_map.end()) {
            const auto& key = itr->second;
            keyToScrambles[key].push_back(scramble);
        }

        if (++counter % 10000 == 0) {
            std::cout << "processed " << counter << "/" << scrambles.size() << " scrambles…" << std::endl;
        }
    }
    return keyToScrambles;
}

void classifyGoodAlgs() {
    const size_t numMoves = 13;

    const auto outputFile = fmt::format("/tmp/zbll_algs_classification.txt", numMoves);
    std::ofstream fs(outputFile);

    const auto zbllCasesMap = loadZbllCasesFromFile("/home/lo/cloud/trash_etc/BLD/zbll/zbll_cases_algs.txt");
    const auto gootZbllAlgs = getFileContentsAsLines("/home/lo/cloud/trash_etc/BLD/zbll/good_zbll_algs.txt");

    uint64_t counter{0};
    std::set<std::string> allAlgs;
    for (const auto& alg : gootZbllAlgs) {
        bool found = false;
        for (const auto& z : {"", " z", " z'", " z2"}) {
            for (const auto& x : {"", " x", " x'", " x2"}) {
                for (const auto& y : {"", " y", " y'", " y2"}) {
                    const auto solution = fmt::format("{}{}{}{}", alg, x, z, y);
                    CubeState cube(3);
                    cube.applyScramble(scrambleTearApart333(invertScramble(solution)));
                    const auto itr = zbllCasesMap.find(cube.toString());
                    if (itr != zbllCasesMap.end()) {
                        if (allAlgs.insert(alg).second) {
                            fs << itr->second << '\t' << alg << '\n';
                        }
                        found = true;
                        break;
                    }
                }
            }
        }
        if (!found) {
            std::cerr << "no zbll case for alg: " << alg << std::endl;
        }

        if (++counter % 100 == 0) {
            std::cout << "processed " << counter << "/" << gootZbllAlgs.size() << std::endl;
        }
    }
    std::cout << "results saved to " << outputFile << std::endl;
}

*/
