#pragma once
#include <unordered_map>
#include <string>

namespace cubing {

static constexpr size_t NUM_STICKERS_ON_ONE_SIDE = 9;
static constexpr size_t NUM_STICKERS_AROUND_CENTER = 8;
static constexpr std::string_view ALGS_FILE_NAME = "algs.txt";
static constexpr std::string_view SCRAMBLE_FILE_NAME = "scramble.txt";

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

struct AlgAndConvenienceScore {
    std::string alg; // wide moves and rotations allowed
    uint32_t convenience_score; // lower is better
};

class PatternToAlgAndConvenienceMap {
public:
    PatternToAlgAndConvenienceMap() = default;
    // calculate convenience score on load
    static PatternToAlgAndConvenienceMap load_from_file(const std::string& path, bool overwrite_with_empty = false);
    [[nodiscard]] bool save_to_file(const std::string& path) const; // don't save convenience scores
    /// @returns true if inserted - TODO change alg type to MovesVector!
    bool insert_if_more_convenient(const std::string& pattern, const std::string& alg);

    size_t size() const {return _map.size();}
    bool empty() const {return _map.empty();}
private:
    std::unordered_map<std::string, AlgAndConvenienceScore> _map;
};



} // namespace