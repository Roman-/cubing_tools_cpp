#include "gtest/gtest.h"
#include "cubing/MovesVector.h"
#include "cubing/IterativeScramble.h"
#include <vector>
#include <string>

using namespace cubing;

TEST(MovesVector, Basic) {
    MovesVector<sides333> v;
    v.push_back(16);
    v.push_back(5);
    std::cout << v.to_string() << '\n';
}
