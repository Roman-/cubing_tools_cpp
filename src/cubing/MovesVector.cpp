#include "MovesVector.h"
#include <strutil.h>
#include <bitset>
#include <fmt/format.h>

namespace cubing {

template<QtmMoveSetSize qtmMoveSetSize>
static std::string move_to_string(uint8_t moveIndex) {
    const uint8_t kNumHtmMoves = qtmMoveSetSize * 3;
    if (moveIndex >= kNumHtmMoves) {
        return "?";
    }
    const size_t prime = moveIndex / qtmMoveSetSize;
    const size_t index = moveIndex % qtmMoveSetSize;
    const char letter = CubeTraits<qtmMoveSetSize>::qtmMoves[index];
    if (prime == directionCw) {
        return std::string(size_t(1), letter); // string ctors suck
    }
    const char prime_char = prime == directionDouble ? '2' : '\'';
    return std::string{letter, prime_char};
}

template<QtmMoveSetSize qtmMoveSetSize>
std::string MovesVector<qtmMoveSetSize>::to_string(bool as_digits) const {
    std::ostringstream ss;
    for (size_t i = 0; i < moves_.size(); ++i) {
        if (i > 0) {
            ss << ' ';
        }
        ss << (as_digits ? std::to_string(int(moves_[i])) : move_to_string<qtmMoveSetSize>(moves_[i]));
    }
    // TODO use join(vector_of_callable) from strutil, but it's TODO in strutil
    return ss.str();
}

/// @returns true for pairs of moves like {R L'}, {S B'}, {D2 E2}, {F F} etc, and false for {F S2}, {R R'}, or {U D}
static bool are_moving_in_same_direction(uint8_t a_htm, uint8_t b_htm) {
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
    // both are single rotations
    const std::bitset<sidesAndMid333> same_direction_as_ruf{0b100000111}; // right-to-left blocks: RUF (same), LDB (not), MES (only S does)
    const bool a_direction = a_prime == RotationDirection::directionCw == same_direction_as_ruf[a_index];
    const bool b_direction = b_prime == RotationDirection::directionCw == same_direction_as_ruf[b_index];
    return a_direction == b_direction;
}

/// @param move - sidesAndMid333 move
static std::string move_to_rotation(uint8_t move) {
    const auto index = move % sidesAndMid333, prime = move / sidesAndMid333;
    const char letter = std::string_view("xyzxyzxyz")[index];
    if (prime == directionDouble) {
        return std::string{letter, '2'};
    }
    const bool direction_matches = std::bitset<sidesAndMid333>{0b111000001}[sidesAndMid333 - index - 1]; // '1' in 0th position means <x> direction matches <R>
    const uint8_t prime_corrected = direction_matches ? prime : ((prime + 2) % 4); // 0 -> 2, 2 -> 0
    if (prime_corrected == directionCw) {
        return std::string(size_t(1), letter); // string ctors suck
    }
    return std::string{letter, '\''};
}

/// @param move - sidesAndMid333 move
static std::string moves_to_wide(uint8_t m1, uint8_t m2) {
    // one of the moves should be non-mid; take it
    const auto i1 = m1 % sidesAndMid333, i2 = m2 % sidesAndMid333;
    const bool pick_m1 = i1 < sides333, pick_m2 = i2 < sides333;
    if (!(pick_m1 ^ pick_m2)) {
        throw std::runtime_error(fmt::format("moves_to_wide: only one move should be side, got {} and {}",
                                             move_to_string<sidesAndMid333>(m1), move_to_string<sidesAndMid333>(m2)));
    }
    const auto index = pick_m1 ? i1 : i2;
    const auto prime = pick_m1 ? m1 / sidesAndMid333 : m2 / sidesAndMid333;
    const char letter = CubeTraits<sidesAndMid333>::qtmMoves[index];
    if (prime == directionCw) {
        return {letter, 'w'};
    }
    const char prime_char = prime == directionDouble ? '2' : '\'';
    return std::string{letter, 'w', prime_char};
}

template<QtmMoveSetSize qtmMoveSetSize>
std::string MovesVector<qtmMoveSetSize>::to_string_combined_moves() const {
    if constexpr (qtmMoveSetSize != sidesAndMid333) {
        throw std::logic_error("to_string_combined_moves is only supported for sidesAndMid333");
    }
    const auto size = moves_.size();
    std::ostringstream ss;
    for (size_t i = 0; i < size; ++i) {
        const bool combined_with_next = (i < size - 1)
                                        && are_moving_in_same_direction(moves_[i], moves_[i+1])
                                        && (moves_[i] != moves_[i+1]);
        // consider F B' S: all 3 moves are parallel but different faces
        const bool combined_with_two_next = combined_with_next
                                            && (i < size - 2)
                                            && are_moving_in_same_direction(moves_[i+1], moves_[i+2])
                                            && (moves_[i] != moves_[i+2])
                                            && (moves_[i+1] != moves_[i+2]);
        const bool this_move_mid = (moves_[i] % qtmMoveSetSize) >= sides333;
        const bool next_move_mid = (i < size - 1) && (moves_[i+1] % qtmMoveSetSize) >= sides333;
        if (combined_with_two_next) {
            ss << move_to_rotation(moves_[i]);
            i += 2;
        } else if (combined_with_next && (this_move_mid ^ next_move_mid)) {
            ss << moves_to_wide(moves_[i], moves_[i+1]);
            i += 1;
        } else {
            ss << move_to_string<qtmMoveSetSize>(moves_[i]);
        }
        if (i != moves_.size() - 1) {
            ss << ' ';
        }
    }
    return ss.str();
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