#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    // update the previous values
    prevTotal = currTotal;
    prevIdle = currIdle;

    // update current values by reading from /proc/stat
    auto stats = LinuxParser::CpuUtilization();
    if (stats.empty()) return 0.0f;

    currIdle = std::stol(stats[LinuxParser::kIdle_]) + std::stol(stats[LinuxParser::kIOwait_]);
    currTotal = 0;

    for (auto stat : stats) {
        currTotal += std::stol(stat);
    }

    auto totald = currTotal - prevTotal;
    auto idled = currIdle - prevIdle;
    return static_cast<float>(totald - idled) / totald;
}

