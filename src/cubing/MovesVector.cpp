#include "MovesVector.h"
#include <strutil.h>

namespace cubing {

template<QtmMoveSetSize qtmMoveSetSize>
static std::string moveToString(uint8_t moveIndex) {
    const uint8_t kNumHtmMoves = qtmMoveSetSize * 3;
    if (moveIndex >= kNumHtmMoves) {
        return "?";
    }
    const size_t prime = moveIndex / qtmMoveSetSize;
    const size_t index = moveIndex % qtmMoveSetSize;
    const char letter = CubeTraits<qtmMoveSetSize>::qtmMoves[index];
    if (prime == 0) {
        return std::string(size_t(1), letter); // string ctors suck
    }
    const char prime_char = prime == 1 ? '2' : '\'';
    return std::string{letter, prime_char};
}

template<QtmMoveSetSize qtmMoveSetSize>
std::string MovesVector<qtmMoveSetSize>::to_string(bool as_digits) const {
    std::ostringstream ss;
    for (size_t i = 0; i < moves_.size(); ++i) {
        if (i > 0) {
            ss << ' ';
        }
        ss << (as_digits ? std::to_string(int(moves_[i])) : moveToString<qtmMoveSetSize>(moves_[i]));
    }
    // TODO use join(vector_of_callable) from strutil, but it's TODO in strutil
    return ss.str();
}

static bool can_sidesAndMid_moves_be_combined(uint8_t a_htm, uint8_t b_htm) {
    constexpr uint8_t qtmMoveSetSize = sidesAndMid333;
    const uint8_t a_prime = a_htm / qtmMoveSetSize, b_prime = b_htm / qtmMoveSetSize, // 0 = CW, 1 = double, 2 = prime
    a_index = a_htm % qtmMoveSetSize, b_index = b_htm % qtmMoveSetSize;
    const uint8_t space_between_parallel_moves = 3; // RUF LDB MES
    if (std::abs(a_index - b_index) % space_between_parallel_moves != 0) {
        return false; // not on parallel layers
    }
    const bool a_is_double = a_prime == RotationDirection::directionDouble;
    const bool b_is_double = b_prime == RotationDirection::directionDouble;
    if (a_is_double && b_is_double) {
        return true; // both double
    }
    if (a_is_double ^ b_is_double) {
        return false; // one is double, another one is not
    }
    const bool a_is_face_turn = a_index < 6, b_is_face_turn = b_index < 6;
    if (a_is_face_turn && b_is_face_turn) {
        return false; // <F B'> is not combined into one; everything else is
    }
    // both are single rotations
    const std::bitset<sidesAndMid333> same_direction_as_ruf{0b100000111}; // right-to-left blocks: RUF (same), LDB (not), MES (only S does)
    const bool a_direction = a_prime == RotationDirection::directionCw == same_direction_as_ruf[a_index];
    const bool b_direction = b_prime == RotationDirection::directionCw == same_direction_as_ruf[b_index];
    return a_direction == b_direction;
}

template<QtmMoveSetSize qtmMoveSetSize>
size_t MovesVector<qtmMoveSetSize>::move_count_combined() const {
    if constexpr (qtmMoveSetSize != sidesAndMid333) {
        throw std::logic_error("move_count_combined is only supported for sidesAndMid333");
    }
    if (moves_.size() < 2) {
        return moves_.size();
    }
    size_t num_moves_that_could_be_combined = 0;
    // consider every adjacent move to find out if they can be combined into one wide move
    for (size_t i = 0; i < moves_.size() - 1; ++i) {
        if (can_sidesAndMid_moves_be_combined(moves_[i], moves_[i + 1])) {
            ++num_moves_that_could_be_combined;
            // do NOT skip next move: for sequences like <L M R'>, num_moves_that_could_be_combined = 2
        }
    }
    return moves_.size() - num_moves_that_could_be_combined;
}

template<QtmMoveSetSize qtmMoveSetSize>
MovesVector<qtmMoveSetSize> MovesVector<qtmMoveSetSize>::from_string(const std::vector<std::string>& scramble_moves) {
    MovesVector<qtmMoveSetSize> scrambleInt;
    for (const auto& moveString : scramble_moves) {
        const auto itr = std::find(CubeTraits<qtmMoveSetSize>::qtmMoves.begin(), CubeTraits<qtmMoveSetSize>::qtmMoves.end(), moveString.front());
        if (itr == CubeTraits<qtmMoveSetSize>::qtmMoves.end()) {
            throw std::runtime_error("from_string: invalid move <" + moveString + ">");
        }
        const uint8_t index = itr - CubeTraits<qtmMoveSetSize>::qtmMoves.begin();
        const uint8_t shift = moveString.size() == 1
                              ? 0 : qtmMoveSetSize * (uint8_t(1) + uint8_t(moveString.back() == '\''));
        scrambleInt.push_back(index + shift);
    }
    return scrambleInt;
}

template<QtmMoveSetSize qtmMoveSetSize>
MovesVector<qtmMoveSetSize> MovesVector<qtmMoveSetSize>::from_string(const std::string& scramble) {
    return scramble.empty() ? MovesVector<qtmMoveSetSize>() : from_string(strutil::split(scramble, ' '));
}

}