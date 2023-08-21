#include "CubeState.h"
#include <algorithm>
#include <strutil.h>
#include <numeric>
#include <vector>
#include <fmt/format.h>
#include "ScrambleProcessing.h"

namespace cubing {

template<QtmMoveSetSize moveSetSize>
bool CubeState<moveSetSize>::isSolved() const {
    return
        (cornersState_.empty() || cornersState_ == cornersStateInitial) &&
        (edgesState_.empty() || edgesState_ == edgesStateInitial) &&
        (xCentersState_.empty() || xCentersState_ == xCentersStateInitial) &&
        (tCentersState_.empty() || tCentersState_ == tCentersStateInitial) &&
        (wingsState_.empty() || wingsState_ == wingsStateInitial) &&
        (capsState_.empty() || capsState_ == capsStateInitial);
}


uint8_t edgeStickerNextIndex(uint8_t index) {
    return (++index % 2 == 0) ? index - 2 : index;
}

template<QtmMoveSetSize moveSetSize>
void CubeState<moveSetSize>::flipEgde(uint8_t edgeNumber) {
    std::swap(edgesState_[edgeNumber * 2], edgesState_[edgeNumber * 2 + 1]);
}

template<QtmMoveSetSize moveSetSize>
void CubeState<moveSetSize>::swapEdges(uint8_t i1, uint8_t i2) {
    if (i1 == i2) {
        return;
    }
    if (i1 / 2 == i2 / 2) { // twist
        return flipEgde(i1 / 2);
    }
    for (int i = 0; i < 2; ++i) {
        std::swap(edgesState_[i1], edgesState_[i2]);
        i1 = edgeStickerNextIndex(i1);
        i2 = edgeStickerNextIndex(i2);
    }
}

template<QtmMoveSetSize moveSetSize>
void CubeState<moveSetSize>::twistCorner(uint8_t cornerNumber, bool clockwise) {
    std::rotate(cornersState_.begin() + cornerNumber * 3,
                clockwise ? (cornersState_.begin() + cornerNumber * 3 + 2) :
                (cornersState_.begin() + cornerNumber * 3 + 1),
                cornersState_.begin() + (cornerNumber + 1) * 3);
}

int cornerStickerNextIndex(int index) {
    return (++index % 3 == 0) ? index - 3 : index;
}

static ssize_t centerStickerIndex(const std::vector<std::string>& config, const std::string& sticker) {
    const auto itrFirstTry = std::find(config.begin(), config.end(), sticker);
    if (itrFirstTry != config.end()) {
        return itrFirstTry - config.begin();
    }
    std::string stickerSpelledDifferently{sticker[0], sticker[2], sticker[1]};
    const auto itrSecondTry = std::find(config.begin(), config.end(), stickerSpelledDifferently);
    return itrSecondTry == config.end() ? -1 : itrSecondTry - config.begin();
}

template<QtmMoveSetSize moveSetSize>
void CubeState<moveSetSize>::swapCorners(uint8_t i1, uint8_t i2) {
    if (i1 == i2) {
        return;
    }
    if (i1 / 3 == i2 / 3) { // twist
        return twistCorner(i1 / 3, (i1 < i2));
    }
    for (int i = 0; i < 3; ++i) {
        std::swap(cornersState_[i1], cornersState_[i2]);
        i1 = cornerStickerNextIndex(i1);
        i2 = cornerStickerNextIndex(i2);
    }
}

template<QtmMoveSetSize moveSetSize>
std::string CubeState<moveSetSize>::toString() const {
/*
    std::array<const std::vector<uint8_t>*, 6> elements{
        &capsState_, &cornersState_, &edgesState_, &xCentersState_, &tCentersState_, &wingsState_};
*/
    std::array<const Elements24State*, 2> elements{&cornersState_, &edgesState_};
    std::string result;
    for (const auto* e : elements)  {
        if (!e->empty()) {
            for (uint8_t value : *e) {
                result.push_back(char('0' + value));
            }
        }
        result.push_back('|');
    }
    result.pop_back();
    return result;
}

template<QtmMoveSetSize moveSetSize>
void CubeState<moveSetSize>::performCornersCycle(const std::vector<uint8_t>& cycle, uint8_t prime) {
    if (cycle.empty() || cornersState_.empty()) {
        return;
    }
    if (prime == 1) { // double
        swapCorners(cycle[0], cycle[2]);
        swapCorners(cycle[1], cycle[3]);
    } else if (prime == 0) { // qtm
        swapCorners(cycle[3], cycle[2]);
        swapCorners(cycle[2], cycle[1]);
        swapCorners(cycle[1], cycle[0]);
    } else if (prime == 2) { // qtm prime
        swapCorners(cycle[0], cycle[1]);
        swapCorners(cycle[1], cycle[2]);
        swapCorners(cycle[2], cycle[3]);
    }
}

template<class ElementsState>
void performCentersCycle(ElementsState& state, const std::vector<uint8_t>& cycle, uint8_t prime) {
    if (cycle.empty() || state.empty()) {
        return;
    }
    if (prime == 1) { // double
        std::swap(state[cycle[0]], state[cycle[2]]);
        std::swap(state[cycle[1]], state[cycle[3]]);
    } else if (prime == 0) { // qtm
        std::swap(state[cycle[3]], state[cycle[2]]);
        std::swap(state[cycle[2]], state[cycle[1]]);
        std::swap(state[cycle[1]], state[cycle[0]]);
    } else if (prime == 2) { // qtm prime
        std::swap(state[cycle[0]], state[cycle[1]]);
        std::swap(state[cycle[1]], state[cycle[2]]);
        std::swap(state[cycle[2]], state[cycle[3]]);
    }
}

template<QtmMoveSetSize moveSetSize>
void CubeState<moveSetSize>::performEdgesCycle(const std::vector<uint8_t>& cycle, uint8_t prime) {
    if (cycle.empty() || edgesState_.empty()) {
        return;
    }
    if (prime == 1) { // double
        swapEdges(cycle[0], cycle[2]);
        swapEdges(cycle[1], cycle[3]);
    } else if (prime == 0) { // qtm
        swapEdges(cycle[3], cycle[2]);
        swapEdges(cycle[2], cycle[1]);
        swapEdges(cycle[1], cycle[0]);
    } else if (prime == 2) { // qtm prime
        swapEdges(cycle[0], cycle[1]);
        swapEdges(cycle[1], cycle[2]);
        swapEdges(cycle[2], cycle[3]);
    }
}


template<QtmMoveSetSize moveSetSize>
void CubeState<moveSetSize>::applyScramble(const std::string& scramble) {
    const std::vector<std::string> initialList = strutil::split(scramble, ' ');
    std::vector<std::string> list; // does not contain rotations
    for (const std::string& move : initialList) {
        if (isRotationLetter(move.front())) {
            const auto moves = rotationToMoves(move, moveSetSize == allMoves555 ? 5 : 3);
            for (const auto& m : moves) {
                list.push_back(m);
            }
        } else if (!move.empty()) {
            list.push_back(move);
        }
    }

    applyScramble(MovesVector<moveSetSize>::from_string(list));
}

template<QtmMoveSetSize moveSetSize>
void CubeState<moveSetSize>::applyScramble(const MovesVector<moveSetSize>& moves) {
    for (const auto m : moves) {
        applyScrambleMove(m);
    }
}

template<QtmMoveSetSize qtmMoveSetSize>
void CubeState<qtmMoveSetSize>::applyScrambleMove(uint8_t move) {
    const uint8_t prime = move / qtmMoveSetSize; // 0: qtm; 1: double; 2: prime
    move = move % qtmMoveSetSize;
    const auto& vec = scrambleMap[move];
    performCornersCycle(vec[0], prime);
    performEdgesCycle(vec[1], prime);
    performCentersCycle(xCentersState_, vec[2], prime);
    performCentersCycle(xCentersState_, vec[3], prime);
    performCentersCycle(tCentersState_, vec[4], prime); // t
    performCentersCycle(tCentersState_, vec[5], prime); // t
    performCentersCycle(wingsState_, vec[6], prime); // w
    performCentersCycle(wingsState_, vec[7], prime); // w
    performCentersCycle(capsState_, vec[8], prime); // caps
}

template<QtmMoveSetSize moveSetSize>
bool CubeState<moveSetSize>::isCorrectlyOriented() const {
    return capsState_.empty() || capsState_ == capsStateInitial;
}

/*
std::string CubeState::rotateUntilCorrectlyOriented() {
    if (isCorrectlyOriented()) {
        return {};
    }
    // quick and dirty solution
    for (const std::string& xRot : std::vector<std::string>{"", "x ", "x' ", "x2 "}) {
        for (const std::string& yRot : std::vector<std::string>{"", "y ", "y' ", "y2 "}) {
            for (const std::string& zRot : std::vector<std::string>{"", "z ", "z' ", "z2 "}) {
                const auto scrambleXyz = strutil::trim_copy(fmt::format("{}{}{}", xRot, yRot, zRot));
                if (scrambleXyz.empty()) {
                    continue;
                }
                const auto scramble = scrambleTearApart333(scrambleXyz);
                applyScramble(scramble);
                if (isCorrectlyOriented()) {
                    return scrambleXyz;
                }
                applyScramble(invertScramble(scramble));
            }
        }
    }
    throw std::runtime_error("centers not restored after all attempts of all the turns");
}
*/

uint8_t mismatches(const std::vector<uint8_t>& v1, const std::vector<uint8_t>& v2) {
    return std::accumulate(v1.begin(), v1.end(), 0, [&v2](uint8_t res, uint8_t v) {
        return res + (v != v2[v]);
    });
}

std::string getElementCycles(std::vector<uint8_t>& state, std::vector<uint8_t>& stateInitial,
                         const std::vector<std::string>& config) {
    if (state.empty()) {
        return {};
    }
    std::string desc;
    while (mismatches(state, stateInitial) > 0) {
        uint8_t i = 0;
        for (;state[i] == i;++i) {} // t[i] != i
        while (state[i] != i) {
            desc += config[state[i]] + "-";
            std::swap(state[i], state[state[i]]);
        }
        desc += config[state[i]] + ".";
    }
    return desc;
}

/*
std::string CubeState::getCycles() {
    return fmt::format("{}{}{}{}{}",
                       getElementCycles(cornersState_, cornersStateInitial, cornersConfig),
                       getElementCycles(edgesState_, edgesStateInitial, edgesConfig),
                       getElementCycles(tCentersState_, tCentersStateInitial, tCentersConfig),
                       getElementCycles(xCentersState_, xCentersStateInitial, xCentersConfig),
                       getElementCycles(wingsState_, wingsStateInitial, wingsConfig));
}
*/

/*
std::string CubeState::whichElementsAreUnsolved() const {
    std::string r;
    // TODO depending on Has...
    for (int i = 0; i < xCentersStateInitial.size(); ++i)
        if (xCentersState_[i] != xCentersStateInitial[i])
            r += xCentersConfig[i] + " ";
    for (int i = 0; i < tCentersStateInitial.size(); ++i)
        if (tCentersState_[i] != tCentersStateInitial[i])
            r += tCentersConfig[i] + " ";
    for (int i = 0; i < wingsStateInitial.size(); ++i)
        if (wingsState_[i] != wingsStateInitial[i])
            r += wingsConfig[i] + " ";

    return r;
}
*/

template<QtmMoveSetSize moveSetSize>
std::string CubeState<moveSetSize>::topSideStickers(bool asColors) const {
    const std::string_view sides = asColors ? "WGROYB" : "UFRLDB";
    return {
        sides[cornersState_[5]], // UBL
        sides[edgesState_[6]], // UB
        sides[cornersState_[8]], // UBR
        sides[edgesState_[2]], // UL
        sides[capsState_[0]], // U
        sides[edgesState_[4]], // UR
        sides[cornersState_[2]], // UFL
        sides[edgesState_[0]], // UF
        sides[cornersState_[11]], // UFR
    };
}

template<QtmMoveSetSize moveSetSize>
std::string CubeState<moveSetSize>::frontSideStickers(bool asColors) const {
    const std::string_view sides = asColors ? "WGROYB" : "UFRLDB";
    return {
        sides[cornersState_[0]], // FUL
        sides[edgesState_[1]], // FU
        sides[cornersState_[10]], // FRU
        sides[edgesState_[16]], // FL
        sides[capsState_[1]], // F
        sides[edgesState_[18]], // FR
        sides[cornersState_[12]], // FDL
        sides[edgesState_[9]], // FD
        sides[cornersState_[23]], // FDR
    };
}

}
