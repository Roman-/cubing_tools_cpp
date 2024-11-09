#include <iostream>
#include "cubing/Mosaic.h"
#include "cubing/CubingDefs.h"
#include "cubing/MosaicDefs.h"
#include "cubing/Helpers.h"
#include <fmt/format.h>
#include <filesystem>

using namespace cubing;

/// if not found, returns empty map
static PatternToAlgMap maybe_load_algs_map_from_file(const std::string& file_path) {
    try {
        return PatternToAlgMap::load_from_file(file_path, false);
    } catch (const std::runtime_error& e) {
        return {}; // ignore errors
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " /path/to/split_dirs" << std::endl;
        exit(-1);
    }
    const auto root_dir = argv[1];

    auto path_to_merged_algs = fmt::format("{}/merged_algs.txt", root_dir);
    if (!saveToFile(path_to_merged_algs, "")) {
        throw std::runtime_error(fmt::format("Failed to create output file {}", path_to_merged_algs));
    }

    // for each dir in root_dir, load algs from algs.txt and merge them
    PatternToAlgMap merged;
    for (const auto& dir : std::filesystem::directory_iterator(root_dir)) {
        if (!std::filesystem::is_directory(dir)) {
            continue;
        }
        const auto path_to_algs = fmt::format("{}/{}", dir.path().string(), ALGS_FILE_NAME);
        std::cout << "Loading algs from " << path_to_algs << std::endl;
        const auto partial_map = maybe_load_algs_map_from_file(path_to_algs);
        if (partial_map.empty()) {
            std::cout << "No algs found in " << path_to_algs << std::endl;
            continue;
        }
        std::cout << "Loaded " << partial_map.size() << " algs, merging..." << std::endl;
        size_t new_algs = 0, shorter_algs = 0;
        for (const auto& [pattern, alg] : partial_map.get()) {
            const bool exists = merged.exists(pattern);
            const bool inserted = merged.insert_if_shorter(pattern, alg);
            new_algs += !exists;
            shorter_algs += exists && inserted;
        }
        std::cout << "Inserted " << new_algs << " new algs and updated " << shorter_algs << " algs\n";
    }

    if (!merged.save_to_file(path_to_merged_algs)) {
        throw std::runtime_error(fmt::format("Failed to save merged algs to {}", path_to_merged_algs));
    }
    std::cout << "Saved " << merged.size() << " algs to " << path_to_merged_algs << '\n';
    return 0;
}
