#include "utils.hpp"
using namespace std;
using namespace log_parser;

bool log_parser::starts_with(const string& str, const string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    } else {
        for (size_t i = 0; i < prefix.length(); i++) {
            if (str[i] != prefix[i])
                return false;
        }

        return true;
    }
}

vector<string> log_parser::split(const string& str, char token) {
    auto result = vector<string>();
    const size_t length = str.length();
    size_t substring_begin = 0;

    for (size_t i = 0; i < length; i++) {
        while (i < length && str[i] == token)
            i++;
        substring_begin = i;
        while (i < length && str[i] != token)
            i++;
        if (substring_begin < i)
            result.emplace_back(str.substr(substring_begin, i - substring_begin));
    }

    return result;
}
