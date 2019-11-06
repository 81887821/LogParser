#include <iostream>
#include <map>
#include "disk.hpp"
#include "utils.hpp"
using namespace std;
using namespace log_parser;
using namespace log_parser::disk;

const size_t SECTOR_SIZE = 512;

int main(int argc, char *argv[]) {
    disk_log logs = read_logs(cin);

    print_logs(logs, cout);
}

disk_log log_parser::disk::read_logs(std::istream& input) {
    set<string> prefix_blacklist = {"loop", "dm-"};
    read_states state = read_states::LOG_START;
    vector<disk_log_row> logs;
    disk_log_row* current_log = nullptr;
    string line;
    unsigned int interval = 0;

    while (getline(input, line)) {
        switch (state) {
            case read_states::LOG_START:
                interval = strtoul(split(line)[6].c_str(), nullptr, 10);
                state = static_cast<read_states>(state + 1);
                break;
            case read_states::TIMESTAMP:
                current_log = &logs.emplace_back();
                current_log->timestamp = move(line);
                state = read_states::LOGS;
                break;
            case read_states::LOGS:
                if (line.empty()) {
                    state = read_states::TIMESTAMP;
                } else {
                    block_log log = parse_line(line);

                    if (!is_blacklisted(prefix_blacklist, log.device))
                        current_log->logs.emplace_back(move(log));
                }
                break;
            default:
                throw std::runtime_error("Invalid read state: " + to_string(state));
        }
    }

    if (state != read_states::TIMESTAMP) {
        cerr << "Reached EOF on state: " << state << endl << "Result might be incomplete" << endl;
    }

    return disk_log {
        .rows = move(logs),
        .logging_interval = interval,
    };
}

bool log_parser::disk::is_blacklisted(const std::set<std::string>& prefix_blacklist, const std::string& device) {
    for (const std::string& prefix: prefix_blacklist) {
        if (starts_with(device, prefix)) {
            return true;
        }
    }

    return false;
}

block_log log_parser::disk::parse_line(const string& line) {
    vector<string> columns = split(line);
    block_log log = {
        .device = move(columns[2]),
        .read_sectors = strtoul(columns[5].c_str(), nullptr, 10),
        .written_sectors = strtoul(columns[9].c_str(), nullptr, 10),
    };

    return log;
}

void log_parser::disk::print_logs(const disk_log& logs, std::ostream& output) {
    if (logs.rows.size() < 2) {
        throw std::runtime_error("log row size is less than 2.");
    }

    auto previous_log = map<string, const block_log*>();

    output << "timestamp";
    for (const block_log& i: logs.rows[0].logs) {
        previous_log[i.device] = &i;
        output << ',' << i.device << "_read(bytes/s)," << i.device << "_written(bytes/s)";
    }
    output << endl;

    for (size_t i = 1; i < logs.rows.size(); i++) {
        output << logs.rows[i].timestamp;
        for (const block_log& log: logs.rows[i].logs) {
            const block_log* previous = previous_log[log.device];
            output << ',' << (log.read_sectors - previous->read_sectors) * SECTOR_SIZE / logs.logging_interval << ',' << (log.written_sectors - previous->written_sectors) * SECTOR_SIZE / logs.logging_interval;
            previous_log[log.device] = &log;
        }
        output << endl;
    }
}
