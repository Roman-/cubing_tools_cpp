#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <array>
#include <unordered_map>
#include "CubingDefs.h"
#include "MovesVector.h"

namespace cubing {

template<QtmMoveSetSize qtmMoveSetSize>
class CubeState {
public:
    explicit CubeState() = default;
    CubeState(const CubeState& other) = default;
    CubeState(CubeState&& other) = default;
    CubeState& operator=(const CubeState& other) = default;
    CubeState& operator=(CubeState&& other) noexcept = default;

    bool isSolved() const;

    /// = caps solved
    bool isCorrectlyOriented() const;

    /// restores caps positions, if any.
    /// @returns sequence of rotations that restore the orientation, e.g. "x y'"
//    std::string rotateUntilCorrectlyOriented();

    /// TODO temporarily only care about 3x3 thing
    std::string toString() const;

    /// @returns 9-char string with each char representing a color (set @param asColors = true) or a face, in this
    /// order: UBL UB UBR UL U UR UFL UF UFR
    std::string topSideStickers(bool asColors = true) const;
    std::string frontSideStickers(bool asColors = true) const;

    /// @returns true if each sticker on the front face has the opposite color as the corresponding sticker on the back face
    bool doFrontAndBackSidesHaveSamePatternWithOppositeColors() const;

    /* scrambling*/
    void applyScramble(const std::string& scramble);

    void applyScramble(const MovesVector<qtmMoveSetSize>& moves);

    void applyScrambleMove(uint8_t move);

    /* corner manipulation */
    void twistCorner(uint8_t cornerNumber, bool clockwise = true);

    void swapCorners(uint8_t i1, uint8_t i2);

    /// \param cycle example: ULB-FUL-DFL-BDL
    void performCornersCycle(const std::vector<uint8_t>& cycle, uint8_t prime);

    /* edge manipulation */
    void flipEgde(uint8_t edgeNumber);

    void swapEdges(uint8_t i1, uint8_t i2);

    /// \param edge cycle example: UL-FU-DF-BD
    void performEdgesCycle(const std::vector<uint8_t>& cycle, uint8_t prime);

    /// \returns true if only a few elements are unsolved
//    bool isInteresting() const;

    /// \returns x,t,w cycles description string. Ex.: Uf-Ur-Br
    std::string getCycles();

//    std::string whichElementsAreUnsolved() const;

private:
    // TODO can remove some of the states with enable_if
    Elements24State cornersState_ = cornersStateInitial;
    Elements24State edgesState_ = edgesStateInitial;
    Elements24State xCentersState_ = xCentersStateInitial;
    Elements24State tCentersState_ = tCentersStateInitial;
    Elements24State wingsState_ = wingsStateInitial;
    Elements6State capsState_ = capsStateInitial;

public:
    static const std::array<std::array<std::vector<uint8_t>, 9>,  qtmMoveSetSize> scrambleMap;
};

template class CubeState<sides333>;
template class CubeState<sidesAndMid333>;
template class CubeState<allMoves555>;

} // namespace cubing