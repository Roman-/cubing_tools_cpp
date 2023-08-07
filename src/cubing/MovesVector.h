#pragma once
#include "CubingDefs.h"
#include <string>

namespace cubing {

// each move is a Htm move
template<QtmMoveSetSize qtmMoveSetSize>
class MovesVector {
public:
    size_t size() const {return moves_.size();}
    void clear() {return moves_.clear();}
    bool empty() {return moves_.empty();}
    void push_back(uint8_t moveIndex) {moves_.push_back(moveIndex);}
    uint8_t& operator[](size_t i) {return moves_[i];}
    const uint8_t& operator[](size_t i) const {return moves_[i];}
    uint8_t& front() {return moves_.front();}
    const uint8_t& front() const {return moves_.front();}
    uint8_t& back() {return moves_.back();}
    const uint8_t& back() const {return moves_.back();}
    // iterators
    auto begin() {return moves_.begin();}
    auto end() {return moves_.end();}
    auto begin() const {return moves_.begin();}
    auto end() const {return moves_.end();}

    std::string to_string(bool as_digits = false) const;
private:
    std::vector<uint8_t> moves_;
};

template class MovesVector<sides333>;
template class MovesVector<sidesAndMid333>;
template class MovesVector<allMoves555>;

} // namespace radix