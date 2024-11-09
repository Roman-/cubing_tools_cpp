#pragma once
#include <string>
#include <vector>
#include <fmt/format.h>
#include <fstream>

std::vector<std::string> getFileContentsAsLines(const std::string& path, bool include_empty_lines = false);

// Overwrites the file if it exists
bool saveToFile(const std::string& path, const std::string& content);
