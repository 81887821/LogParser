#ifndef LOGPARSER_DISK_HPP
#define LOGPARSER_DISK_HPP
#include <string>
#include <set>
#include <vector>
#include <istream>
#include <ostream>

namespace log_parser::disk {
    struct block_log {
        std::string device;
        unsigned long read_sectors;
        unsigned long written_sectors;
    };

    struct disk_log_row {
        std::string timestamp;
        std::vector<block_log> logs;
    };

    struct disk_log {
        std::vector<disk_log_row> rows;
        unsigned int logging_interval;
    };

    enum read_states {LOG_START, TIMESTAMP, LOGS};

    disk_log read_logs(std::istream& input);
    bool is_blacklisted(const std::set<std::string>& prefix_blacklist, const std::string& device);
    block_log parse_line(const std::string& line);
    void print_logs(const disk_log& logs, std::ostream& output);
}

#endif //LOGPARSER_DISK_HPP
