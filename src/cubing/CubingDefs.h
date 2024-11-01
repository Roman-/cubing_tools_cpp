#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <stdexcept>
#include <sstream>

namespace cubing {

// Having QtmMoveSetSize defined, a single uint8 number represents a move. For sides333: 0 = R, 5 = B, 6 = R2, 12 = R', 17 = B'
enum QtmMoveSetSize : uint8_t {
    sides333 = 6, // RUFLDB
    sidesAndMid333 = 9, //RUFLDB MES
    allMoves555 = 15 // RUFLDB MES rufldb
};

// You get RotationDirection when you divide uint8 move by QtmMoveSetSize, e.g. direction_of(uint8(7)) = 7/6 = 1 (double)
enum RotationDirection : uint8_t {
    directionCw = 0,
    directionDouble = 1,
    directionCcw = 2
};

template<QtmMoveSetSize>
struct CubeTraits {
    static const std::string_view qtmMoves;
    static inline bool are_parallel_layer_moves(uint8_t m1, uint8_t m2) {return (std::abs(m1-m2) % 3 == 0);}
};
template class CubeTraits<sides333>;
template class CubeTraits<sidesAndMid333>;
template class CubeTraits<allMoves555>;

template<typename OStream>
OStream& operator<<(OStream& os, const QtmMoveSetSize& setSize) {
    switch (setSize) {
        case QtmMoveSetSize::sides333: os << "sides333"; return os;
        case QtmMoveSetSize::sidesAndMid333: os << "sidesAndMid333"; return os;
        case QtmMoveSetSize::allMoves555: os << "allMoves555"; return os;
        default: throw std::runtime_error("Unknown QtmMoveSetSize");
    }
}

using Elements24State = std::array<uint8_t, 24>;
using Elements6State = std::array<uint8_t, 6>;

// each digit represents a color, from 0 to 5: wgroyb. Positions match those from cornersConfig/edgesConfig.
static Elements24State cornersStateInitial = {1,3,0,3,5,0,5,2,0,2,1,0,1,4,3,3,4,5,5,4,2,2,4,1};
static Elements24State edgesStateInitial = {0,1,0,3,0,2,0,5,4,1,4,3,4,2,4,5,1,3,1,2,5,3,5,2};

static Elements24State xCentersStateInitial = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
static Elements24State tCentersStateInitial = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
static Elements24State wingsStateInitial = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
static Elements6State capsStateInitial = {0,1,2,3,4,5};

static const std::vector<std::string> cornersConfig =
    { "FUL", "LFU", "ULF", "LUB", "BLU", "UBL", "BUR", "RBU", "URB", "RUF", "FRU", "UFR", "FLD", "DFL", "LDF", "LBD", "DLB", "BDL", "BRD", "DBR", "RDB", "RFD", "DRF", "FDR"};
    //  0      1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23

static const std::vector<std::string> edgesConfig =
    { "UF", "FU", "UL", "LU", "UR", "RU", "UB", "BU", "DF", "FD", "DL", "LD", "DR", "RD", "DB", "BD", "FL", "LF", "FR", "RF", "BL", "LB", "BR", "RB"};
    // 0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21    22    23

static const std::vector<std::string> xCentersConfig =
    { "Ful", "Lfu", "Ulf", "Lub", "Blu", "Ubl", "Bur", "Rbu", "Urb", "Ruf", "Fru", "Ufr", "Fld", "Dfl", "Ldf", "Lbd", "Dlb", "Bdl", "Brd", "Dbr", "Rdb", "Rfd", "Drf", "Fdr"};
    //  0      1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23

static const std::vector<std::string> tCentersConfig =
    { "Uf", "Fu", "Ul", "Lu", "Ur", "Ru", "Ub", "Bu", "Df", "Fd", "Dl", "Ld", "Dr", "Rd", "Db", "Bd", "Fl", "Lf", "Fr", "Rf", "Bl", "Lb", "Br", "Rb"};
    // 0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21    22    23

static const std::vector<std::string> wingsConfig =
    { "UFl", "FUr", "ULb", "LUf", "URf", "RUb", "UBr", "BUl", "DFr", "FDl", "DLf", "LDb", "DRb", "RDf", "DBl", "BDr", "FLu", "LFd", "FRd", "RFu", "BLd", "LBu", "BRu", "RBd"};
    //  0      1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23

static const std::vector<std::string> capsConfig =
    {"U", "F", "R", "L", "D", "B"};
   // 0    1    2    3    4    5


} // namespace cubing