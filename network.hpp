#ifndef LOGPARSER_NETWORK_HPP
#define LOGPARSER_NETWORK_HPP
#include <string>
#include <vector>
#include <istream>
#include <ostream>

namespace log_parser::network {
    struct interface_log {
        std::string interface;
        unsigned long received_bytes;
        unsigned long transmitted_bytes;
    };

    struct network_log_row {
        std::string timestamp;
        std::vector<interface_log> logs;
    };

    struct network_log {
        std::vector<network_log_row> rows;
        unsigned int logging_interval;
    };

    enum read_states {LOG_START, TIMESTAMP, HEADER1, HEADER2, LOGS};

    network_log read_logs(std::istream& input);
    interface_log parse_line(const std::string& line);
    void print_logs(const network_log& logs, std::ostream& output);
}

#endif //LOGPARSER_NETWORK_HPP
