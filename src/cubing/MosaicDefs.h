#pragma once
#include <unordered_map>
#include <string>

namespace cubing {

static constexpr size_t NUM_STICKERS_ON_ONE_SIDE = 9;
static constexpr std::string_view ALGS_FILE_NAME = "algs.txt";

class PatternToAlgMap {
public:
    PatternToAlgMap() = default;
    // allow implicit init
    PatternToAlgMap(const std::unordered_map<std::string, std::string>& m) : _map(m) {}
    static PatternToAlgMap load_from_file(const std::string& path, bool overwrite_with_empty = false);
    [[nodiscard]] bool save_to_file(const std::string& path) const;
    bool exists(const std::string& pattern) const;
    /// @returns true if inserted
    bool insert(const std::string& pattern, const std::string& alg);
    bool insert_if_shorter(const std::string& pattern, const std::string& alg);
    std::unordered_map<std::string, std::string>& get() {return _map;}
    const std::unordered_map<std::string, std::string>& get() const {return _map;}

    size_t size() const;
    bool empty() const {return _map.empty();}
private:
    std::unordered_map<std::string, std::string> _map;
};


} // namespace