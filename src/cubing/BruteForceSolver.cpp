#include "BruteForceSolver.h"
#include <iostream>

/*
namespace cubing {
BruteForceSolver::BruteForceSolver(const CubeState& stateToSolve) :
    initialState_(stateToSolve) {}

void BruteForceSolver::solve() {
    CubeState state = initialState_;
    uint8_t currentDepth = 0;

    while (!state.isSolved()) {
        state = initialState_;
        ++iterativeScramble_;
        state.applyScramble(iterativeScramble_.get());
        if (iterativeScramble_.size() != currentDepth) {
            currentDepth = iterativeScramble_.size();
            if (currentDepth > 2) {
                // TODO output elapsed time in human-readable format
                std::cout << "searching depth " << iterativeScramble_.size() << std::endl;
            }
        }
    }

     std::cout << "solution found: " << iterativeScramble_.toString();
}

}

 */