#include "ScrambleProcessing.h"
#include <strutil.h>
#include <fmt/format.h>
#include <numeric>

namespace cubing {

// returns cycles inversed.
// cycles - cycles representation like "UF-UL-UR"
// "UF-UL-UR" -> "UR-UL-UF"
std::string inverseCycles(const std::string& cyclesString) {
    std::vector<std::string> cycles = strutil::split(cyclesString, '.');
    for (std::string& cycle : cycles) {
        auto cycleParts = strutil::split(cycle, '-');
        std::reverse(cycleParts.begin(), cycleParts.end());
        cycle = strutil::join(cycleParts, "-");
    }
    return strutil::join(cycles, ".");
}

void removeMiscChars(std::string& s) {
    const std::string miscChars = "\".,()";
    // remove all miscChars from string
    for (char c : miscChars) {
        s.erase(std::remove(s.begin(), s.end(), c), s.end());
    }
    // replace multiple spaces with single space
    strutil::replace_all(s, "  ", " ");
}

std::string normalize(const std::string& source) {
    std::string s = strutil::trim_copy(source);
    removeMiscChars(s);

    std::string apostrophes = "ʼ᾿՚’`";
    for (char a : apostrophes) {
        for (char& c : s) {
            c = (c == a) ? '\'' : c;
        }
    }

    // replace "r'2 to r2"
    strutil::replace_all(s, "'2", "2");
    return s;
}

bool is34CubeMove(const std::string& m) {
    return std::regex_search(m, std::regex{"^[34][RLFBDU]w[\'2]?$"});
}

bool isCubeMove(const std::string& m) {
    return std::regex_search(m, std::regex{"^[RLFBDU]w?[\'2]?$"});
}

bool isSmallCubeMove(const std::string& m) {
    return std::regex_search(m, std::regex{"^[rlfbdu][\'2]?$"});
}

bool isSliceCubeMove(const std::string& m) {
    return std::regex_search(m, std::regex{"^[SMExyz][\'2]?$"});
}

std::string invertScramble(const std::string& scramble) {
    std::vector<std::string> l = strutil::split(scramble, ' ');
    std::reverse(l.begin(), l.end());
    std::transform(l.begin(), l.end(), l.begin(), invertMove);
    return strutil::join(l, " ");
}

std::string invertMove(const std::string& move) {
    if (move.empty() || move.back() == '2') {
        return move;
    }
    if (move.back() == '\'' || move.back() == '3') {
        return move.substr(0, move.size() - 1);
    }
    return move + "'";
}

std::string fromRokickiNotation(const std::string& s) {
    if (s.empty() || s.size() % 2 != 0) {
        return {};
    }
    auto rockPrimeToPrime = [](char c) {
        switch (c) {
            case '-' :
                return "'";
            case '2' :
                return "2";
            default:
            case '+' :
                return "";
        }
    };
    std::vector<std::string> moves;
    for (size_t i = 0; i < s.size(); i += 2) {
        moves.push_back(fmt::format("{}{}", s[i], rockPrimeToPrime(s[i + 1])));
    }
    return strutil::join(moves, " "); // TODO ' ' in strutil
}

bool isRotationLetter(char move) {
    return move == 'x' || move == 'y' || move == 'z';
}

std::vector<std::string> rotationToMoves(const std::string& rotation, uint8_t cubeSize) {
    if (cubeSize != 3) {
        throw std::runtime_error("rotationToMoves only works with 333 cube now");
    }
    const std::vector<std::pair<std::string, std::vector<std::string>>> map = {
        {"x2", {"R2", "M2", "L2"}},
        {"x'", {"R'", "M",  "L"}},
        {"x",  {"R",  "M'", "L'"}},

        {"y2", {"U2", "E2", "D2"}},
        {"y'", {"U'", "E",  "D"}},
        {"y",  {"U",  "E'", "D'"}},

        {"z2", {"F2", "S2", "B2"}},
        {"z'", {"F'", "S'", "B"}},
        {"z",  {"F",  "S",  "B'"}},
    };
    for (const auto& [moveCandidate, result] : map) {
        if (moveCandidate == rotation) {
            return result;
        }
    }
    return {};
}

using MirrorMap = std::unordered_map<std::string, std::string>;
static MirrorMap l2rMap{
    // TODO for other moves (x, M, 3Rw' etc.), look in bld_tools_strings project
    {"R",  "L'"},
    {"R'", "L"},
    {"U",  "U'"},
    {"D",  "D'"},
    {"F",  "F'"},
    {"B",  "B'"},

    {"L'", "R"},
    {"L",  "R'"},
    {"U'", "U"},
    {"D'", "D"},
    {"F'", "F"},
    {"B'", "B"},

    {"L2", "R2"},
    {"R2", "L2"},
    {"F2", "F2"},
    {"B2", "B2"},
    {"D2", "D2"},
    {"U2", "U2"},
};

static MirrorMap f2bMap{
    // TODO for other moves (x, M, 3Rw' etc.), look in bld_tools_strings project
    {"R",  "R'"},
    {"R'", "R"},
    {"U",  "U'"},
    {"D",  "D'"},
    {"F",  "B'"},
    {"B",  "F'"},

    {"L'", "L"},
    {"L",  "L'"},
    {"U'", "U"},
    {"D'", "D"},
    {"F'", "B"},
    {"B'", "F"},

    {"L2", "L2"},
    {"R2", "R2"},
    {"F2", "B2"},
    {"B2", "F2"},
    {"D2", "D2"},
    {"U2", "U2"},
};

static std::string mirrorMove(const std::string& m, const MirrorMap& map) {
    const auto itr = map.find(m);
    if (itr == map.end()) {
        throw std::runtime_error(fmt::format("while mirroring alg, got unknown move <{}>", m));
    }
    return itr == map.end() ? m : itr->second;
}

std::string mirrorAlg(const std::string& alg, const MirrorMap& map) {
    auto moves = strutil::split(alg, ' ');
    std::vector<std::string> result;
    result.reserve(moves.size());
    for (std::string& m : moves) {
        result.push_back(mirrorMove(m, map));
    }
    return strutil::join(result, " ");
}

std::string left2right(const std::string& alg) {
    return mirrorAlg(alg, l2rMap);
}
std::string front2back(const std::string& alg) {
    return mirrorAlg(alg, f2bMap);
}

std::string scrambleTearApart333(const std::string& input_alg) {
    std::string alg = input_alg;
    strutil::replace_all(alg, "2'", "2");
    strutil::replace_all(alg, "'2", "2");
    strutil::replace_all(alg, "3", "'");

    strutil::replace_all(alg, "Rw", "r");
    strutil::replace_all(alg, "Lw", "l");
    strutil::replace_all(alg, "Fw", "f");
    strutil::replace_all(alg, "Bw", "b");
    strutil::replace_all(alg, "Uw", "u");
    strutil::replace_all(alg, "Dw", "d");

    strutil::replace_all(alg, "r2", "R2 M2");
    strutil::replace_all(alg, "r\'", "R\' M");
    strutil::replace_all(alg, "r", "R M\'");

    strutil::replace_all(alg, "l2", "L2 M2");
    strutil::replace_all(alg, "l\'", "L\' M\'");
    strutil::replace_all(alg, "l", "L M");

    strutil::replace_all(alg, "u2", "U2 E2");
    strutil::replace_all(alg, "u\'", "U\' E");
    strutil::replace_all(alg, "u", "U E\'");

    strutil::replace_all(alg, "d2", "D2 E2");
    strutil::replace_all(alg, "d\'", "D\' E\'");
    strutil::replace_all(alg, "d", "D E");

    strutil::replace_all(alg, "f2", "F2 S2");
    strutil::replace_all(alg, "f\'", "F\' S\'");
    strutil::replace_all(alg, "f", "F S");

    strutil::replace_all(alg, "b2", "B2 B2");
    strutil::replace_all(alg, "b\'", "B\' S");
    strutil::replace_all(alg, "b", "B S\'");

    return alg;
}

static std::unordered_map<char, uint32_t> move_scores = {
    {'R', 100},
    {'U', 100},
    {'F', 130},
    {'L', 110},
    {'D', 120},
    {'B', 150},
    {'M', 120},
    {'E', 150},
    {'S', 150},
    {'r', 120},
    {'u', 120},
    {'f', 160},
    {'l', 130},
    {'d', 140},
    {'b', 170},
    {'x', 145},
    {'y', 150},
    {'z', 150},
};

uint32_t execution_convenience_score(const std::vector<std::string>& moves) {
    auto single_move_execution_convenience_score = [](const std::string& move) {
        if (move.empty()) {
            throw std::runtime_error("execution_convenience_score: empty string");
        }
        const auto itr = move_scores.find(move.front());
        if (itr == move_scores.end()) {
            throw std::runtime_error("execution_convenience_score: unknown move string <" + move + ">");
        }
        uint32_t base_score = itr->second, wide_penalty = 0, double_penalty = 0;
        if (move.size() > 1 && move[1] == 'w') {
            wide_penalty = 15 + (base_score - 100) / 2; // R=100 => Rw=115; B=150 => Bw=150+15+25=190
        }
        if (move.back() == '2') {
            double_penalty = 10 + (base_score-100) / 4; // R=100, R2=110; D=120, D2=135; E=150, E2=172
        }
        return base_score + wide_penalty + double_penalty;
    };

    return std::accumulate(moves.begin(), moves.end(), 0, [&](uint32_t sum, const std::string& move) {
        return sum + single_move_execution_convenience_score(move);
    });
}

uint32_t execution_convenience_score(const std::string& alg) {
    if (alg.empty()) {
        return 0;
    }
    return execution_convenience_score(strutil::split(alg, ' '));
}


/*
std::string scrambleGlueMoves333(const std::string& suboptimal_alg) {
    auto alg = suboptimal_alg;
    alg = std::regex_replace(alg, std::regex("R2 M2 L2"), "x2");
    alg = std::regex_replace(alg, std::regex("R M' L'"), "x");
    alg = std::regex_replace(alg, std::regex("R' M L(?=\\s|$)"), "x'");

    alg = std::regex_replace(alg, std::regex("U2 E2 D2"), "y2");
    alg = std::regex_replace(alg, std::regex("U E' D'"), "y");
    alg = std::regex_replace(alg, std::regex("U' E D(?=\\s|$)"), "y'");

    alg = std::regex_replace(alg, std::regex("F2 S2 B2"), "z2");
    alg = std::regex_replace(alg, std::regex("F S B'"), "z");
    alg = std::regex_replace(alg, std::regex("F' S' B(?=\\s|$)"), "z'");

    alg = std::regex_replace(alg, std::regex("R2 M2"), "Rw2");
    alg = std::regex_replace(alg, std::regex("R M'"), "Rw");
    alg = std::regex_replace(alg, std::regex("R' M(?=\\s|$)"), "Rw'");

    alg = std::regex_replace(alg, std::regex("L2 M2"), "Lw2");
    alg = std::regex_replace(alg, std::regex("L' M'"), "Lw'");
    alg = std::regex_replace(alg, std::regex("L M(?=\\s|$)"), "Lw");

    alg = std::regex_replace(alg, std::regex("U2 E2"), "Uw2");
    alg = std::regex_replace(alg, std::regex("U' E(?=\\s|$)"), "Uw'");
    alg = std::regex_replace(alg, std::regex("U E'"), "Uw");

    alg = std::regex_replace(alg, std::regex("F2 S2"), "Fw2");
    alg = std::regex_replace(alg, std::regex("F' S'"), "Fw'");
    alg = std::regex_replace(alg, std::regex("F S(?=\\s|$)"), "Fw");

    // Replace wide move notations
    alg = std::regex_replace(alg, std::regex("B' S(?=\\s|$)"), "Bw'");
    alg = std::regex_replace(alg, std::regex("B S'"), "Bw");
    alg = std::regex_replace(alg, std::regex("B2 S2"), "Bw2");

    alg = std::regex_replace(alg, std::regex("D E(?=\\s|$)"), "Dw");
    alg = std::regex_replace(alg, std::regex("D' E'"), "Dw'");
    alg = std::regex_replace(alg, std::regex("D2 E2"), "Dw2");
    return alg;
}
*/

/*
void scrambleTearApart555(std::string& alg) {
    alg = alg.replace("2\'", "2").replace("\'2", "2");
    alg = alg.replace("4Uw2", "3Uw2 d2").replace("4Uw\'", "3Uw\' l").replace("4Uw", "3Uw l\'");
    alg = alg.replace("4Rw2", "3Rw2 l2").replace("4Rw\'", "3Rw\' l").replace("4Rw", "3Rw l\'");
    alg = alg.replace("4Lw2", "3Lw2 r2").replace("4Lw\'", "3Lw\' r").replace("4Lw", "3Lw r\'");
    alg = alg.replace("3Rw2", "Rw2 M2").replace("3Rw\'", "Rw\' M").replace("3Rw", "Rw M\'");
    alg = alg.replace("3Lw2", "Lw2 M2").replace("3Lw\'", "Lw\' M\'").replace("3Lw", "Lw M");
    alg = alg.replace("3Uw2", "Uw2 E2").replace("3Uw\'", "Uw\' E").replace("3Uw", "Uw E\'");
    alg = alg.replace("3Dw2", "Dw2 E2").replace("3Dw\'", "Dw\' E\'").replace("3Dw", "Dw E");
    //alg = alg.replace("f2", "F2 S2").replace("f\'", "F\' S\'").replace("f", "F S");
}

void scrambleGlueMoves555(std::string &alg) {
    alg = alg.replace("Rw2 M2", "3Rw2").replace("Rw\' M", "3Rw\'").replace("Rw M\'", "3Rw");
    alg = alg.replace("Lw2 M2", "3Lw2").replace("Lw\' M\'", "3Lw\'").replace("Lw M", "3Lw");
    alg = alg.replace("3Uw2 d2", "4Uw2").replace("3Uw\' d", "4Uw\'").replace("3Uw d\'", "4Uw");
    alg = alg.replace("3Rw2 l2", "4Rw2").replace("3Rw\' l", "4Rw\'").replace("3Rw l\'", "4Rw");
    alg = alg.replace("3Lw2 r2", "4Lw2").replace("3Lw\' r", "4Lw\'").replace("3Lw r\'", "4Lw");
    alg = alg.replace("Uw2 E2", "3Uw2").replace("Uw\' E", "3Uw\'").replace("Uw E\'", "3Uw");
    alg = alg.replace("Dw2 E2", "3Dw2").replace("Dw\' E\'", "3Dw\'").replace("Dw E", "3Dw");
    //alg = alg.replace("F2 S2", "f2").replace("F\' S\'", "f\'").replace("F S", "f");

    alg = alg.replace("R2 r2 M2 l2 L2", "x2");
    alg = alg.replace("R\' r\' M L l", "x\'");
    alg = alg.replace("R r M\' L\' l\'", "x");

    alg = alg.replace("U2 u2 E2 D2 d2", "y2");
    alg = alg.replace("U\' u\' E D d", "y\'");
    alg = alg.replace("U u E\' D\' d\'", "y");

    alg = alg.replace("F2 f2 S2 s2 B2 b2", "z2");
    alg = alg.replace("F\' f\' S\' B b", "z\'");
    alg = alg.replace("F f S B\' b\'", "z");
}

void scrambleReplaceRotations555(std::string &alg) {
    // todo for 5bld add inner layers
    alg = alg.replace("x2", "R2 r2 M2 l2 L2");
    alg = alg.replace("x\'", "R\' r\' M L l");
    alg = alg.replace("x", "R r M\' L\' l\'");

    alg = alg.replace("y2", "U2 u2 E2 D2 d2");
    alg = alg.replace("y\'", "U\' u\' E D d");
    alg = alg.replace("y", "U u E\' D\' d\'");

    alg = alg.replace("z2", "F2 f2 S2 s2 B2 b2");
    alg = alg.replace("z\'", "F\' f\' S\' B b");
    alg = alg.replace("z", "F f S B\' b\'");
}

void scrambleReplaceRotations333(std::string &alg) {
    // todo for 5bld add inner layers
    alg = alg.replace("x2", "R2 M2 L2");
    alg = alg.replace("x\'", "R\' M L");
    alg = alg.replace("x", "R M\' L\'");

    alg = alg.replace("y2", "U2 E2 D2");
    alg = alg.replace("y\'", "U\' E D");
    alg = alg.replace("y", "U E\' D\'");

    alg = alg.replace("z2", "F2 S2 B2");
    alg = alg.replace("z\'", "F\' S\' B");
    alg = alg.replace("z", "F S B\'");
}

std::string fixStupidInputHabits(std::string alg) {
    std::string apostrophesChars = "ʼ᾿՚’`";
    for (char& ap: apostrophesChars)
        alg.replace(std::string(1, ap), "\'");

    std::unordered_map<std::string, std::string> replacements = {

        // do people still use parentheses?
        {")", ""},
        {"(", ""},

        // Ollie uses asterisk lol
        {"*", ""},

        // for those who type R2'
        {"\'2", "2"},
        {"2\'", "2"},
        {"3", "4"}, // for those who only solve 4bld and assume 4Lw = 3Lw

        // Oliver's stupid 2Lw
        {"2Rw", "Rw"},
        {"2Lw", "Lw"},
        {"2Uw", "Uw"},
        {"2Dw", "Dw"},
        {"2Bw", "Bw"},

        // Daniel's spliced moves
        // too tired to define general rule, I will go as I encounter keys
        {"3RwL", "3Rw L"},
        {"DU", "D U"},
        {"D\'U", "D\' U"},
        {"D\'U", "D\' U"},
        {"FB", "F B"},
        {"L\'M", "L\' M"},
        {"Ll", "L l"},
        {"lL", "l L"},
        {"Lr", "L r"},
        {"Rl", "R l"},
        {"RL", "R L"},
        {"Lw\'r", "Lw\' r"},
        {"RM", "R M"},
        {"R\'D", "R\' D"},
        {"Rr", "R r"},
        {"Rwr", "Rw r"},
        {"UD", "U D"},
        {"2D", "2 D"},
        {"UD", "U D"},
        {"UE", "U E"},
        {"U\'D", "U\' D"},
        {"U\'Dw", "U\' Dw"},
        {"U\'E", "U\' E"},
        {"U\'d", "U\' d"},
        {"U\'u", "U\' u"},
        {"UwU", "Uw U"},
        {"l\'M", "l\' M"},
        {"rM", "r M"},
        {"r\'M", "r\' M"},
    };
    for (auto p = replacements.begin(); p != replacements.end(); ++p) {
        alg = alg.replace(p.key(), p.value());
    }
    return alg;
}

*/

} // namespace cubing