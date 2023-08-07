#pragma once

#include <string>
#include <vector>

namespace cubing {

// delete double spaces, leave moves only. "R  U' (2f)" -> "R U'"
std::string normalize(const std::string &source);

/// \returns true if \param m is side-move (R,U,L,D,B,F), R', U', ..., R2, U2, ..., F2.
bool isCubeMove(const std::string &m);

/// \returns true if \param m is slice-move (S,M,E), S', ..., S2, M2, F2.
bool isSliceCubeMove(const std::string &m);

/// \returns true if \param m is small cube move (r,u,l,d,b,f), r', u', ..., r2, u2, ..., f2.
bool isSmallCubeMove(const std::string &m);

/// \returns true if \param m is 3Rw/4Rw/3Uw... cube move
bool is34CubeMove(const std::string &m);

/// \returns inverted STM move
std::string invertMove(const std::string& move);

/// \returns inverted scramble
std::string invertScramble(const std::string& scramble);

/// \param scrambleInRokicki : something like F+U2L-U2R+U2R-U2L+F-
std::string fromRokickiNotation(const std::string& scrambleInRokicki);

/// \returns true for move = 'x', 'y' and 'z'
bool isRotationLetter(char move);

std::vector<std::string> rotationToMoves(const std::string& rotation, uint8_t cubeSize);

/// converts algorithm left-to-right. Example: R U R' D -> L' U' L D'
std::string left2right(const std::string& alg);

/// converts algorithm front-to-back
std::string front2back(const std::string& alg);

/// \replace moves like 'r' "R M'" etc.
std::string scrambleTearApart333(const std::string& alg);
/*
/// \replace moves like '3Rw' with "Rw M'" etc.
void scrambleTearApart555(std::string& scramble);

/// \replace moves like "R M'" with 'r' etc.
void scrambleGlueMoves555(std::string& scramble);


/// \replace moves like 'r' "R M'" etc.
void scrambleGlueMoves333(std::string& scramble);

/// replace rotation moves like "x" with R M' L' etc. on 5x5 cube
void scrambleReplaceRotations555(std::string& scramble);

/// replace rotation moves like "x" with R M' L' etc. on 5x5 cube
void scrambleReplaceRotations333(std::string& scramble);

/// inverses cycles in cyclesString. Example: "UF-UL-UR" -> "UR-UL-UF"
std::string inverseCycles(const std::string& cyclesString);

/// tear apart spliced moves (U'D'), replaces '2 with 2
std::string fixStupidInputHabits(std::string alg);

*/

} // namespace cubing