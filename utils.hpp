#ifndef LOGPARSER_UTILS_HPP
#define LOGPARSER_UTILS_HPP
#include <string>
#include <vector>

namespace log_parser {
    bool starts_with(const std::string& str, const std::string& prefix);
    std::vector<std::string> split(const std::string& str, char token = ' ');
}

#endif //LOGPARSER_UTILS_HPP
