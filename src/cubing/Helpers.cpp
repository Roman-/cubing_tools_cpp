#include "Helpers.h"

std::vector<std::string> getFileContentsAsLines(const std::string& path, bool include_empty_lines) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return {};
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

bool saveToFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    file.close();
    return file.good();
}
