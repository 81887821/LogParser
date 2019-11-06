#include <iostream>
#include <set>
#include <map>
#include "network.hpp"
#include "utils.hpp"
using namespace std;
using namespace log_parser;
using namespace log_parser::network;

int main() {
    network_log logs = read_logs(cin);

    print_logs(logs, cout);
}

network_log log_parser::network::read_logs(std::istream& input) {
    set<string> blacklist = {"lo"};
    read_states state = read_states::LOG_START;
    vector<network_log_row> logs;
    network_log_row* current_log = nullptr;
    string line;
    unsigned int interval = 0;

    while (getline(input, line)) {
        switch (state) {
            case read_states::LOG_START:
                interval = strtoul(split(line)[6].c_str(), nullptr, 10);
            case read_states::HEADER1:
            case read_states::HEADER2:
                state = static_cast<read_states>(state + 1);
                break;
            case read_states::TIMESTAMP:
                current_log = &logs.emplace_back();
                current_log->timestamp = move(line);
                state = read_states::HEADER1;
                break;
            case read_states::LOGS:
                if (line.empty()) {
                    state = read_states::TIMESTAMP;
                } else {
                    interface_log log = parse_line(line);

                    if (blacklist.count(log.interface) == 0)
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

    return network_log {
        .rows = move(logs),
        .logging_interval = interval,
    };
}

interface_log log_parser::network::parse_line(const string& line) {
    vector<string> columns = split(line);
    interface_log log = {
        .interface = columns[0].substr(0, columns[0].length() - 1),
        .received_bytes = strtoul(columns[1].c_str(), nullptr, 10),
        .transmitted_bytes = strtoul(columns[9].c_str(), nullptr, 10),
    };

    return log;
}

void log_parser::network::print_logs(const network_log& logs, std::ostream& output) {
    if (logs.rows.size() < 2) {
        throw std::runtime_error("log row size is less than 2.");
    }

    auto previous_log = map<string, const interface_log*>();

    output << "timestamp";
    for (const interface_log& i: logs.rows[0].logs) {
        previous_log[i.interface] = &i;
        output << ',' << i.interface << "_received(bytes/s)," << i.interface << "_transmitted(bytes/s)";
    }
    output << endl;

    for (size_t i = 1; i < logs.rows.size(); i++) {
        output << logs.rows[i].timestamp;
        for (const interface_log& log: logs.rows[i].logs) {
            const interface_log* previous = previous_log[log.interface];
            output << ',' << (log.received_bytes - previous->received_bytes) / logs.logging_interval << ',' << (log.transmitted_bytes - previous->transmitted_bytes) /
                logs.logging_interval;
            previous_log[log.interface] = &log;
        }
        output << endl;
    }
}
