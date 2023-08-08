#pragma once
#include <cstdint>
#include <string>
#include "CubingDefs.h"
#include "MovesVector.h"

namespace cubing {

template<QtmMoveSetSize qtmMoveSetSize>
class IterativeScramble {
public:
    // Next algorithm. Skips algs like <R R2>. Also skips <M' R L'> and <L' R M'> but not <R L' M'> (ascending move indexes)
    IterativeScramble& operator++();

    /// \returns progress report (percent, num moves etc.)
    std::string progress() const;

    /// \returns current scramble
    const MovesVector<qtmMoveSetSize>& get() const {return moves_;}

    /// resets the scramble
    void reset() {moves_.clear();}

    /// \returns current alg size
    std::size_t size() const {return moves_.size();}

    static IterativeScramble<qtmMoveSetSize> from_moves(const MovesVector<qtmMoveSetSize>& moves);

private:
    MovesVector<qtmMoveSetSize> moves_;
};

template class IterativeScramble<sides333>;
template class IterativeScramble<sidesAndMid333>;
template class IterativeScramble<allMoves555>;

}
