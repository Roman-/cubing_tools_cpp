#include <fstream>
#include <filesystem>
#include <fmt/format.h>
#include "MosaicDefs.h"
#include "ScrambleProcessing.h"

namespace cubing {

PatternToAlgMap PatternToAlgMap::load_from_file(const std::string& path, bool overwrite_with_empty) {
    if (!std::filesystem::exists(path)) {
        if (overwrite_with_empty) {
            // make sure we'll be able to use file later for writing
            std::ofstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error(fmt::format("PatternToAlgMap: failed to open file {} for writing", path));
            }
            file.close();
        }
        return {};
    }
    std::ifstream alg_file(path);
    if (!alg_file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open the file {}", path));
    }
    std::unordered_map<std::string, std::string> result;
    std::string line;
    while (std::getline(alg_file, line)) {
        if (line.size() <= NUM_STICKERS_ON_ONE_SIDE || line[NUM_STICKERS_ON_ONE_SIDE] != '\t') {
            continue;
        }
        const auto stickers = line.substr(0, NUM_STICKERS_ON_ONE_SIDE);
        const auto alg = line.substr(NUM_STICKERS_ON_ONE_SIDE + 1);
        result.insert({stickers, alg});
    }
    alg_file.close();
    return {result};
}

bool PatternToAlgMap::save_to_file(const std::string& path) const {
    std::ofstream alg_file(path);
    if (!alg_file.is_open()) {
        return false;
    }
    for (const auto& [pattern, alg] : _map) {
        alg_file << pattern << '\t' << alg << '\n';
    }
    alg_file.close();
    return alg_file.good();
}

bool PatternToAlgMap::exists(const std::string& pattern) const {
    return _map.find(pattern) != _map.end();
}

bool PatternToAlgMap::insert(const std::string& pattern, const std::string& alg) {
    return _map.insert({pattern, alg}).second;
}

size_t PatternToAlgMap::size() const {
    return _map.size();
}

bool PatternToAlgMap::insert_if_shorter(const std::string& pattern, const std::string& alg) {
    // can't use _map[alg], because it may be empty (for GGGGGGGGG pattern) which is valid
    const auto itr = _map.find(pattern);
    if (itr == _map.end() || itr->second.size() > alg.size()) {
        _map.insert({pattern, alg});
        return true;
    }
    return false;
}

PatternToAlgAndConvenienceMap PatternToAlgAndConvenienceMap::load_from_file(const std::string& path, bool overwrite_with_empty) {
    auto m = PatternToAlgMap::load_from_file(path, overwrite_with_empty);
    PatternToAlgAndConvenienceMap result;
    for (const auto& [pattern, alg] : m.get()) {
        result._map.insert({pattern, {alg, execution_convenience_score(alg)}});
    }
    return result;
}

bool PatternToAlgAndConvenienceMap::save_to_file(const std::string& path) const {
    std::ofstream alg_file(path);
    if (!alg_file.is_open()) {
        return false;
    }
    for (const auto& [pattern, alg_and_score] : _map) {
        alg_file << pattern << '\t' << alg_and_score.alg << '\n';
    }
    alg_file.close();
    return alg_file.good();
}

bool PatternToAlgAndConvenienceMap::insert_if_more_convenient(const std::string& pattern, const std::string& alg) {
    const auto itr = _map.find(pattern);
    if (itr == _map.end()) {
        _map.insert({pattern, {alg, execution_convenience_score(alg)}});
        return true;
    }
    if (const auto new_score = execution_convenience_score(alg); new_score < itr->second.convenience_score) {
        itr->second.alg = alg;
        itr->second.convenience_score = new_score;
        return true;
    }
    return false;
}

}