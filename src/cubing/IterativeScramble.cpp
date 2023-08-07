#include "IterativeScramble.h"
#include <sstream>

namespace cubing {

template<QtmMoveSetSize qtmMoveSetSize>
IterativeScramble<qtmMoveSetSize>& IterativeScramble<qtmMoveSetSize>::operator++() {
    if (moves_.empty()) {
        moves_.push_back(0);
        return *this;
    }
    auto areMovesOfSameLayer = [this](uint8_t m1, uint8_t m2) { return (m1 % qtmMoveSetSize == m2 % qtmMoveSetSize); };
    const auto numHtmMoves = qtmMoveSetSize * 3; // U, U2, U'
    auto incrementStartingFrom = [&](size_t i) {
        ++moves_[i];
        while (moves_[i] >= numHtmMoves) {
            moves_[i] = 0;
            if (i == moves_.size() - 1) {
                moves_[i] = 0;
                moves_.push_back(0);
            } else {
                ++i;
                ++moves_[i];
            }
        }
    };

    incrementStartingFrom(0);

    for (size_t j = 0; j < moves_.size() - 1; ++j) {
        if (areMovesOfSameLayer(moves_[j], moves_[j + 1])) {
            incrementStartingFrom(j);
            j = -1; // reset
        }
    }

    return *this;
}

template<QtmMoveSetSize qtmMoveSetSize>
std::string IterativeScramble<qtmMoveSetSize>::progress() const {
    const size_t size = moves_.size();
    if (size < 3) {
        return "instant";
    }
    const auto kNumHtmMoves = qtmMoveSetSize * 3;
    static const int k1 = kNumHtmMoves, k2 = k1 * kNumHtmMoves, k3 = k2 * kNumHtmMoves;
    static const double maxProgress = k3 + k2 + k1;
    double it = moves_[size - 1] * k2
                + moves_[size - 2] * k1
                + moves_[size - 3];
    std::string result = std::to_string(size) + " moves, " +
                         std::to_string(it / maxProgress * 100) + "%";
    return result;
}

}
