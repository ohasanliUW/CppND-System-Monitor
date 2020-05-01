#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

#include <iostream>

using std::set;
using std::size_t;
using std::string;
using std::vector;

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
    auto pids = LinuxParser::Pids();

    // Walk old processes and remove the ones that do not exist anymore
    processes_.erase(std::remove_if(processes_.begin(),
                     processes_.end(),
                     [pids](Process &p) {return pids.end() == std::find(pids.begin(), pids.end(), p.Pid());}),
                     processes_.end());

    for (auto pid : pids) {
        if (processes_.end() == std::find_if(processes_.begin(), processes_.end(), [pid](Process &p) {
            return p.Pid() == pid;
        })) {
            processes_.emplace_back(pid);
        }
    }

    // Before returning all processes, we need to sort them by CPU utilization
    // First, we calculate CPU utilization for each Process then we sort them and return
    std::for_each(processes_.begin(), processes_.end(), [](Process &p){p.CpuUtilization();});
    std::sort(processes_.begin(), processes_.end(), std::greater<Process>());
    return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() {
    return LinuxParser::Kernel();
}

// DONE: Return the system's memory utilization
float System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

// DONE: Return the operating system name
std::string System::OperatingSystem() {
    return LinuxParser::OperatingSystem();
}

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// DONE: Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// DONE: Return the number of seconds since the system started running
long int System::UpTime() {
    return LinuxParser::UpTime();
}
