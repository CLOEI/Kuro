#pragma once
#include <string>
#include <unordered_map>
#include <vector>

inline std::vector<std::string> parse_as_vec(std::string str, const std::string &delimiter) {
    std::vector<std::string> result;
    size_t pos = 0;

    while ((pos = str.find(delimiter)) != std::string::npos) {
        std::string token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    result.push_back(str);
    return result;
}

inline std::unordered_map<std::string, std::string> parse_and_store_as_map(std::string &str, const std::string &delimiter) {
    std::unordered_map<std::string, std::string> result;
    size_t pos = 0;

    while ((pos = str.find(delimiter)) != std::string::npos) {
        const std::string key = str.substr(0, pos);
        str.erase(0, pos + 1);
        pos = str.find('\n');
        const std::string value = str.substr(0, pos);
        result[key] = value;
        str.erase(0, pos + 1);
    }
    return result;
}

inline std::string map_join_with_delimiter(const std::unordered_map<std::string, std::string> &map, const std::string &delimiter) {
    std::string result;
    for (const auto &pair : map) {
        result += pair.first + delimiter + pair.second + '\n';
    }
    return result;
}