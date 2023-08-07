#include "gtest/gtest.h"
#include "cubing/MovesVector.h"
#include <vector>
#include <string>
#include <strutil.h>

using namespace cubing;

TEST(MovesVector, ToString) {
    const std::vector<std::string> scrambles = {"R U R'", "F U R B2 S M"};
    for (const auto& initial : scrambles) {
        const auto v = MovesVector<sidesAndMid333>::from_string(initial);
        const std::string result = v.to_string(false);
        ASSERT_EQ(result, initial);
    }
}

TEST(MovesVector, MoveCount) {
    MovesVector<sidesAndMid333> v;

//    v.push_back(...);
    v.push_back(5);
    std::cout << v.to_string() << '\n';
}
