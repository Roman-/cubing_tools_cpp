#pragma once
#include <cstdint>
#include <string>
#include "CubingDefs.h"
#include "MovesVector.h"

namespace cubing {

template<QtmMoveSetSize qtmMoveSetSize>
class IterativeScramble {
public:
    // Next algorithm. Skips algs like <R R2>.
    IterativeScramble& operator++();

    /// \returns progress in percent
    std::string progress() const;

    /// \returns current scramble
    const MovesVector<qtmMoveSetSize>& get() const {return moves_;}

    /// resets the scramble
    void reset() {moves_.clear();}

    /// \returns current alg size
    std::size_t size() const {return moves_.size();}

private:
    MovesVector<qtmMoveSetSize> moves_;
};

template class IterativeScramble<sides333>;
template class IterativeScramble<sidesAndMid333>;
template class IterativeScramble<allMoves555>;

}
