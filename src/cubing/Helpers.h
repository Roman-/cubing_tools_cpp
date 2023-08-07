#pragma once
#include <string>
#include <vector>
#include <fmt/format.h>
#include <fstream>

// result may include empty
static std::vector<std::string> getFileContentsAsLines(const std::string& path, bool include_empty_lines = false) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open the file {}", path));
    }
    std::vector<std::string> result;
    std::string line;
    auto is_empty = [](const std::string& line) {
        return line.empty() || std::all_of(line.begin(), line.end(), [](char c) { return std::isspace(c); });
    };
    while (std::getline(file, line)) {
        if (include_empty_lines || !is_empty(line)) {
            result.push_back(line);
        }
    }
    file.close();
    return result;
}

