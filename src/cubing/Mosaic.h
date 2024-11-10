#pragma once
#include <string>
#include <chrono>

namespace cubing {

static auto now = [] {
    return std::chrono::steady_clock::now();
};

void doMosaicMesTest(int argc, char** argv);
void doTwoSidedMosaicTest(int argc, char** argv);

} // namespace cubing