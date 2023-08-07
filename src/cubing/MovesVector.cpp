#include "MovesVector.h"

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


}